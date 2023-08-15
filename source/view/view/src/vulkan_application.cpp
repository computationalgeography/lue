#include "lue/view/vulkan_application.hpp"
#include "lue/configure.hpp"
#include "lue/glfw.hpp"
#include "lue/imgui.hpp"
#include <fmt/format.h>
#include <cassert>
#include <cmath>
#include <set>

#include <iostream>


namespace lue::view {
    namespace {

        static VKAPI_ATTR VkBool32 VKAPI_CALL debug_report(
            [[maybe_unused]] VkDebugReportFlagsEXT flags,
            VkDebugReportObjectTypeEXT object_type,
            [[maybe_unused]] std::uint64_t object,
            [[maybe_unused]] std::size_t location,
            [[maybe_unused]] std::int32_t message_code,
            [[maybe_unused]] char const* layer_prefix,
            char const* message,
            [[maybe_unused]] void* user_data)
        {
            std::cerr << fmt::format(
                             "[vulkan] Debug report from ObjectType: {}: Message: {}", object_type, message)
                      << std::endl;

            return VK_FALSE;
        }


        static VkSurfaceFormatKHR select_swap_surface_format(
            vulkan::PhysicalDevice::SurfaceProperties::Formats const& formats)
        {
            assert(!formats.empty());

            for (auto const& format : formats)
            {
                if (format.format == VK_FORMAT_B8G8R8A8_SRGB &&
                    format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                {
                    return format;
                }
            }

            // Default, for now
            return formats[0];
        }


        static VkPresentModeKHR select_swap_present_mode(
            vulkan::PhysicalDevice::SurfaceProperties::PresentModes const& present_modes)
        {
            for (auto const& present_mode : present_modes)
            {
                if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
                {
                    return present_mode;
                }
            }

            // Default, always available
            return VK_PRESENT_MODE_FIFO_KHR;
        }


        static VkExtent2D select_swap_extent(
            VkSurfaceCapabilitiesKHR const& capabilities, std::tuple<int, int> const& framebuffer_size)
        {
            VkExtent2D extent{};

            if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
            {
                // Current extent
                return capabilities.currentExtent;
            }
            else
            {
                // Actual extent
                // Bound the extent to the min/max extents supported by the implementation
                extent = {
                    std::clamp(
                        static_cast<uint32_t>(std::get<0>(framebuffer_size)),
                        capabilities.minImageExtent.width,
                        capabilities.maxImageExtent.width),
                    std::clamp(
                        static_cast<uint32_t>(std::get<1>(framebuffer_size)),
                        capabilities.minImageExtent.height,
                        capabilities.maxImageExtent.height)};
            }

            return extent;
        }


        static vulkan::Surface create_surface(vulkan::Instance& instance, glfw::Window& window)
        {
            VkSurfaceKHR surface;

            VkResult result{::glfwCreateWindowSurface(instance, window, nullptr, &surface)};

            if (result != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create window surface");
            }

            return vulkan::Surface{instance, surface};
        }

    }  // Anonymous namespace


    VulkanApplication::VulkanApplication(std::vector<std::string> const& arguments):

        Application{arguments},
        _library{},
        _monitor{},
        _window{},
        _binding{},
        _enable_validation_layers{false},
        _instance{},
        _debug_callback{},
        _surface{},
        _physical_device{},
        _device{},
        _graphics_queue{},
        _present_queue{},
        _swapchain{},
        _swapchain_images{},
        _image_format{},
        _image_extent{},
        _swapchain_image_views{},
        _render_pass{},
        _pipeline_layout{},
        _graphics_pipeline{},
        _framebuffers{},
        _command_pool{},
        _command_buffer{},
        _image_available_semaphore{},
        _render_finished_semaphore{},
        _in_flight_fence{}

    {
#ifndef NDEBUG
        // Enable validation layers (only) in debug builds
        _enable_validation_layers = true;
#endif
    }


    VulkanApplication::~VulkanApplication()
    {
    }


    void VulkanApplication::init_window()
    {
        // This stuff is needed to be able to draw to a window. When drawining to a file,
        // this should not be needed.

        _library = std::make_unique<glfw::Library>();
        _monitor = std::make_unique<glfw::Monitor>();

        assert(glfwVulkanSupported());

        // Create window with Vulkan context
        glfw::Window::hint(GLFW_CLIENT_API, GLFW_NO_API);
        glfw::Window::hint(GLFW_RESIZABLE, GLFW_FALSE);  // TODO

        glfw::VideoMode const video_mode{_monitor->video_mode()};

        glfw::Window::hint(GLFW_RED_BITS, video_mode.red_bits());
        glfw::Window::hint(GLFW_GREEN_BITS, video_mode.green_bits());
        glfw::Window::hint(GLFW_BLUE_BITS, video_mode.blue_bits());
        glfw::Window::hint(GLFW_REFRESH_RATE, video_mode.refresh_rate());

        _window = std::make_unique<glfw::Window>(
            "LUE view",
            // Screen coordinates. Note that Vulkan works with pixels.
            static_cast<int>(std::ceil(0.9 * video_mode.width())),
            static_cast<int>(std::ceil(0.9 * video_mode.height())));
    }


    void VulkanApplication::init_vulkan()
    {
        create_instance();

        if (_enable_validation_layers)
        {
            setup_debug_messenger();
        }

        create_surface();
        pick_physical_device();
        create_logical_device();
        create_swapchain();
        create_image_views();
        create_render_pass();
        create_graphics_pipeline();
        create_framebuffers();
        create_command_pool();
        create_command_buffer();
        create_sync_objects();
    }


    void VulkanApplication::init_imgui()
    {
        VkPipelineCache pipeline_cache{VK_NULL_HANDLE};

        int const nr_images = _swapchain_images.size();

        // TODO
        // _binding = std::make_unique<imgui::glfw::VulkanBinding>(
        //         *_window, _instance, _physical_device, _device,
        //         _graphics_queue, pipeline_cache, nr_images
        //     );
    }


    vulkan::Names VulkanApplication::required_extension_names() const
    {
        // TODO Optional, if a window is needed
        vulkan::Names extension_names{glfw::Library::required_instance_extensions()};
        // / Optional, if a window is needed

        if (_enable_validation_layers)
        {
            extension_names.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
        }

        // On macOS / Molten, creating an instance may result in VK_ERROR_INCOMPATIBLE_DRIVER. This
        // can be solved:
        // If using MacOS with the latest MoltenVK sdk, you may get VK_ERROR_INCOMPATIBLE_DRIVER
        // returned from vkCreateInstance. According to the Getting Start Notes. Be-
        // ginning with the 1.3.216 Vulkan SDK, the VK_KHR_PORTABILITY_subset
        // extension is mandatory.
        // - Add VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR to CreateInfo's flags
        // - Add VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME to extension_names
        // TODO Can we know beforehand if doing this is required and do it conditionally?
        // - Test for macOS OS + MoltenVK SDK + Vulkan SDK version >= 1.3.216

        return extension_names;
    }


    vulkan::Names VulkanApplication::required_layer_names() const
    {
        vulkan::Names layer_names{};

        if (_enable_validation_layers)
        {
            vulkan::Instance::LayerProperties const available_layer_properties{
                vulkan::Instance::layer_properties()};

            // Only available when the LunarG Vulkan SDK is installed
            if (vulkan::Instance::layer_available(available_layer_properties, "VK_LAYER_KHRONOS_validation"))
            {
                layer_names.push_back("VK_LAYER_KHRONOS_validation");
            }
            else
            {
                throw std::runtime_error{"Warning: VK_LAYER_KHRONOS_validation layer not available. "
                                         "Install the LunarG Vulkan SDK and its validation layers."};
            }
        }

        return layer_names;
    }


    void VulkanApplication::create_instance()
    {
        std::string const application_name{"lue_view"};

#ifndef NDEBUG
        {
            vulkan::Names vulkan_extension_names{glfw::Library::required_instance_extensions()};

            std::cout << "GLFW required Vulkan extension names:";

            for (auto const& name : vulkan_extension_names)
            {
                std::cout << " " << name;
            }

            std::cout << std::endl;

            auto const [variant, major, minor, patch] = vulkan::Instance::version();

            std::cout << "Vulkan API version: " << major << "." << minor << "." << patch;

            if (variant != 0)
            {
                std::cout << " (variant: " << variant << ")";
            }

            std::cout << std::endl;

            vulkan::ExtensionProperties const extension_properties{vulkan::Instance::extension_properties()};

            std::cout << "Available extensions:" << std::endl;

            for (auto const& properties : extension_properties)
            {
                std::cout << "    " << properties.extensionName << std::endl;
            }

            vulkan::Instance::LayerProperties const layer_properties{vulkan::Instance::layer_properties()};

            std::cout << "Available layers:" << std::endl;

            for (auto const& properties : layer_properties)
            {
                std::cout << "    " << properties.layerName << ": " << properties.description << std::endl;
            }
        }
#endif

        // TODO Add debug print callback to validate create/destroy the instance itself

        vulkan::ApplicationInfo application_info{
            application_name,
            std::make_tuple(
                BuildOptions::major_version, BuildOptions::minor_version, BuildOptions::patch_version),
            VK_API_VERSION_1_2};

        vulkan::Instance::CreateInfo instance_create_info{};

        (*instance_create_info).flags = VkInstanceCreateFlags{};
        (*instance_create_info).pApplicationInfo = application_info;

        auto const layer_names = required_layer_names();

        (*instance_create_info).enabledLayerCount = layer_names.size();
        (*instance_create_info).ppEnabledLayerNames = layer_names.data();

        auto const extension_names = required_extension_names();

        (*instance_create_info).enabledExtensionCount = extension_names.size();
        (*instance_create_info).ppEnabledExtensionNames = extension_names.data();

        _instance = vulkan::Instance{instance_create_info};
    }


    void VulkanApplication::setup_debug_messenger()
    {
        vulkan::DebugReportCallback::CreateInfo create_info{};

        (*create_info).flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT |
                               VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
        (*create_info).pfnCallback = debug_report;

        _debug_callback = vulkan::DebugReportCallback{_instance, std::move(create_info)};
    }


    void VulkanApplication::create_surface()
    {
        // Create surface, right after creating the instance and configuring the debugging
        // messenger. Otherwise the other code may behave differently, given that we created
        // a surface.

        _surface = vulkan::Surface{lue::view::create_surface(_instance, *_window)};
    }


    vulkan::QueueFamilies VulkanApplication::find_queue_families(
        vulkan::PhysicalDevice const& physical_device)
    {
        vulkan::QueueFamilyProperties queue_family_properties{physical_device.queue_family_properties()};
        vulkan::QueueFamilies queue_families{};

        for (std::uint32_t i = 0; i < queue_family_properties.size(); ++i)
        {
            // Queue family supporting graphics and presentation can be the same family, or
            // not. Here, we don't care yet, but in the future we may want to prefer this.

            if (queue_family_properties[i].graphics())
            {
                queue_families.set_graphics_family(vulkan::QueueFamily{i});
            }

            if (physical_device.has_surface_support(vulkan::QueueFamily{i}, _surface))
            {
                queue_families.set_present_family(vulkan::QueueFamily{i});
            }

            if (queue_families.is_complete())
            {
                break;
            }
        }

        return queue_families;
    }


    bool VulkanApplication::physical_device_is_suitable(vulkan::PhysicalDevice const& device)
    {
        vulkan::QueueFamilies queue_families = find_queue_families(device);

        vulkan::Names required_extension_names{};
        required_extension_names.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        bool const extensions_supported = device.extensions_available(required_extension_names);

        bool swap_chain_is_adequate{false};

        if (extensions_supported)
        {
            auto const surface_properties = device.surface_properties(_surface);
            swap_chain_is_adequate =
                !surface_properties.formats().empty() && !surface_properties.present_modes().empty();
        }

        return queue_families.is_complete() && extensions_supported && swap_chain_is_adequate;
    }


    void VulkanApplication::pick_physical_device()
    {
        // Look for and select the graphics card(s) that support(s) the features that we need
        vulkan::PhysicalDevices physical_devices{_instance.physical_devices()};

        if (physical_devices.empty())
        {
            throw std::runtime_error("Failed to find GPUs with Vulkan support");
        }

#ifndef NDEBUG
        std::cout << "Physical devices:" << std::endl;

        for (auto const& device : physical_devices)
        {
            auto const properties{device.properties()};
            auto const features{device.features()};

            std::cout << "    " << properties.device_name() << std::endl;
            std::cout << "        has geometry shader: " << features.has_geometry_shader() << std::endl;
        }
#endif

        for (auto& device : physical_devices)
        {
            if (physical_device_is_suitable(device))
            {
                _physical_device = std::move(device);
                break;
            }
        }

        if (!_physical_device)
        {
            throw std::runtime_error("Failed to find a suitable GPU!");
        }

#ifndef NDEBUG
        std::cout << "Physical device selected:" << std::endl;

        auto const properties{_physical_device.properties()};
        auto const features{_physical_device.features()};

        std::cout << "    " << properties.device_name() << std::endl;
        std::cout << "        has geometry shader: " << features.has_geometry_shader() << std::endl;
#endif
    }


    void VulkanApplication::create_logical_device()
    {
        vulkan::QueueFamilies queue_families{find_queue_families(_physical_device)};
        assert(queue_families.is_complete());

        // Create logical device, for each unique queue family. The family supporting graphics
        // and the one supporting presentation are likely to be the same. Use a set to merge them.
        std::set<vulkan::QueueFamily> unique_queue_families{
            queue_families.graphics_family(), queue_families.present_family()};

        std::vector<vulkan::Device::QueueCreateInfo> queue_create_infos(unique_queue_families.size());

        std::transform(
            unique_queue_families.begin(),
            unique_queue_families.end(),
            queue_create_infos.begin(),
            [](vulkan::QueueFamily const& queue_family)
            {
                static float const queue_priority{1.0f};
                vulkan::Device::QueueCreateInfo create_info{};

                (*create_info).queueFamilyIndex = queue_family;
                (*create_info).queueCount = 1;
                (*create_info).pQueuePriorities = &queue_priority;

                return create_info;
            });

        vulkan::Names required_extension_names{};
        required_extension_names.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        vulkan::Device::CreateInfo device_create_info{};

        // We are passing a pointer to the Vulkan structure that is layered in our QueueCreateInfo
        // type. Internally, this pointer will be moved to reach the next info instance. This
        // should work when the Vulkan structure and our type have the same size. This is the case
        // when our structure does not have additional data members.
        static_assert(
            sizeof(vulkan::Device::QueueCreateInfo) == sizeof(vulkan::Device::QueueCreateInfo::VkType));
        (*device_create_info).pQueueCreateInfos = *queue_create_infos.data();
        (*device_create_info).queueCreateInfoCount = queue_create_infos.size();
        (*device_create_info).pEnabledFeatures = nullptr;
        (*device_create_info).enabledExtensionCount = required_extension_names.size();
        (*device_create_info).ppEnabledExtensionNames = required_extension_names.data();

        _device = vulkan::Device{_physical_device, device_create_info};

        // Possibly the same queue
        _graphics_queue = vulkan::Device::Queue{_device.queue(queue_families.graphics_family())};
        _present_queue = vulkan::Device::Queue{_device.queue(queue_families.present_family())};
    }


    void VulkanApplication::create_swapchain()
    {
        vulkan::PhysicalDevice::SurfaceProperties surface_properties{
            _physical_device.surface_properties(_surface)};

        VkSurfaceFormatKHR const swap_surface_format{
            select_swap_surface_format(surface_properties.formats())};
        VkPresentModeKHR const present_mode{select_swap_present_mode(surface_properties.present_modes())};
        VkExtent2D const swap_extent{
            select_swap_extent(surface_properties.capabilities(), _window->framebuffer_size())};

        std::uint32_t nr_images = surface_properties.capabilities().minImageCount + 1;

        if (surface_properties.capabilities().maxImageCount > 0 &&
            nr_images > surface_properties.capabilities().maxImageCount)
        {
            nr_images = surface_properties.capabilities().maxImageCount;
        }

        VkSurfaceTransformFlagBitsKHR const transform{surface_properties.capabilities().currentTransform};

        vulkan::QueueFamilies const queue_families{find_queue_families(_physical_device)};

        vulkan::Swapchain::CreateInfo swapchain_create_info{};

        (*swapchain_create_info).surface = _surface;
        (*swapchain_create_info).minImageCount = nr_images;
        (*swapchain_create_info).imageFormat = swap_surface_format.format;
        (*swapchain_create_info).imageColorSpace = swap_surface_format.colorSpace;
        (*swapchain_create_info).imageExtent = swap_extent;
        (*swapchain_create_info).imageArrayLayers = 1;
        (*swapchain_create_info).imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        assert(queue_families.is_complete());
        std::uint32_t queue_family_indices[] = {
            queue_families.graphics_family(), queue_families.present_family()};

        if (queue_families.graphics_family() != queue_families.present_family())
        {
            (*swapchain_create_info).imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            (*swapchain_create_info).queueFamilyIndexCount = 2;
            (*swapchain_create_info).pQueueFamilyIndices = queue_family_indices;
        }
        else
        {
            (*swapchain_create_info).imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            (*swapchain_create_info).queueFamilyIndexCount = 0;      // Optional
            (*swapchain_create_info).pQueueFamilyIndices = nullptr;  // Optional
        }

        (*swapchain_create_info).preTransform = transform;
        (*swapchain_create_info).compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        (*swapchain_create_info).presentMode = present_mode;
        (*swapchain_create_info).clipped = VK_TRUE;
        (*swapchain_create_info).oldSwapchain = VK_NULL_HANDLE;

        _swapchain = _device.swapchain(swapchain_create_info);

        _swapchain_images = _swapchain.images();
        _image_format = swap_surface_format.format;
        _image_extent = swap_extent;
    }


    void VulkanApplication::create_image_views()
    {
        _swapchain_image_views.resize(_swapchain_images.size());

        for (std::size_t i = 0; i < _swapchain_images.size(); ++i)
        {
            vulkan::ImageView::CreateInfo create_info{};

            (*create_info).image = _swapchain_images[i];
            (*create_info).viewType = VK_IMAGE_VIEW_TYPE_2D;
            (*create_info).format = _image_format;
            (*create_info).components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            (*create_info).components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            (*create_info).components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            (*create_info).components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            (*create_info).subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            (*create_info).subresourceRange.baseMipLevel = 0;
            (*create_info).subresourceRange.levelCount = 1;
            (*create_info).subresourceRange.baseArrayLayer = 0;
            (*create_info).subresourceRange.layerCount = 1;

            _swapchain_image_views[i] = _device.image_view(create_info);
        }
    }


    void VulkanApplication::create_render_pass()
    {
        // Attachment description
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = _image_format;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        // Subpasses and attachment references
        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        // Subpass dependencies
        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;

        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        // Render pass
        vulkan::RenderPass::CreateInfo create_info{};
        (*create_info).attachmentCount = 1;
        (*create_info).pAttachments = &colorAttachment;
        (*create_info).subpassCount = 1;
        (*create_info).pSubpasses = &subpass;
        (*create_info).dependencyCount = 1;
        (*create_info).pDependencies = &dependency;

        _render_pass = _device.render_pass(create_info);
    }


    void VulkanApplication::create_graphics_pipeline()
    {
        // TODO Where to put these files?
        vulkan::ShaderModule::Bytes vertex_shader_code{
            vulkan::ShaderModule::read_file("source/view/view/shader/shader.vert.spv")};
        vulkan::ShaderModule::Bytes fragment_shader_code{
            vulkan::ShaderModule::read_file("source/view/view/shader/shader.frag.spv")};

        vulkan::ShaderModule::CreateInfo vertex_shader_module_create_info{};

        (*vertex_shader_module_create_info).codeSize = vertex_shader_code.size();
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        (*vertex_shader_module_create_info).pCode =
            reinterpret_cast<std::uint32_t const*>(vertex_shader_code.data());

        vulkan::ShaderModule vertex_shader_module{_device.shader_module(vertex_shader_module_create_info)};

        vulkan::ShaderModule::CreateInfo fragment_shader_module_create_info{};

        (*fragment_shader_module_create_info).codeSize = fragment_shader_code.size();
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        (*fragment_shader_module_create_info).pCode =
            reinterpret_cast<std::uint32_t const*>(fragment_shader_code.data());

        vulkan::ShaderModule fragment_shader_module{
            _device.shader_module(fragment_shader_module_create_info)};

        vulkan::Pipeline::ShaderStageCreateInfo vertex_shader_stage_info{};
        (*vertex_shader_stage_info).stage = VK_SHADER_STAGE_VERTEX_BIT;
        (*vertex_shader_stage_info).module = vertex_shader_module;
        (*vertex_shader_stage_info).pName = "main";

        vulkan::Pipeline::ShaderStageCreateInfo fragment_shader_stage_info{};
        (*fragment_shader_stage_info).stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        (*fragment_shader_stage_info).module = fragment_shader_module;
        (*fragment_shader_stage_info).pName = "main";

        VkPipelineShaderStageCreateInfo shader_stages[] = {
            *vertex_shader_stage_info, *fragment_shader_stage_info};

        vulkan::Pipeline::VertexInputStateCreateInfo vertex_input_state_create_info{};
        (*vertex_input_state_create_info).vertexBindingDescriptionCount = 0;
        (*vertex_input_state_create_info).pVertexBindingDescriptions = nullptr;  // Optional
        (*vertex_input_state_create_info).vertexAttributeDescriptionCount = 0;
        (*vertex_input_state_create_info).pVertexAttributeDescriptions = nullptr;  // Optional

        vulkan::Pipeline::InputAssemblyStateCreateInfo input_assembly_state_create_info{};
        (*input_assembly_state_create_info).topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        (*input_assembly_state_create_info).primitiveRestartEnable = VK_FALSE;

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)_image_extent.width;
        viewport.height = (float)_image_extent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = _image_extent;

        VkPipelineDynamicStateCreateInfo dynamic_state_create_info{};
        dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        std::vector<VkDynamicState> dynamic_states = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
        dynamic_state_create_info.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
        dynamic_state_create_info.pDynamicStates = dynamic_states.data();

        VkPipelineViewportStateCreateInfo viewport_state_create_info{};
        viewport_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_state_create_info.viewportCount = 1;
        viewport_state_create_info.scissorCount = 1;

        // Rasterizer
        VkPipelineRasterizationStateCreateInfo rasterization_state_create_info{};
        rasterization_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterization_state_create_info.depthClampEnable = VK_FALSE;
        rasterization_state_create_info.rasterizerDiscardEnable = VK_FALSE;
        rasterization_state_create_info.polygonMode = VK_POLYGON_MODE_FILL;
        rasterization_state_create_info.lineWidth = 1.0f;
        rasterization_state_create_info.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterization_state_create_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterization_state_create_info.depthBiasEnable = VK_FALSE;
        rasterization_state_create_info.depthBiasConstantFactor = 0.0f;  // Optional
        rasterization_state_create_info.depthBiasClamp = 0.0f;           // Optional
        rasterization_state_create_info.depthBiasSlopeFactor = 0.0f;     // Optional


        // Multisampling
        VkPipelineMultisampleStateCreateInfo multisample_state_create_info{};
        multisample_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisample_state_create_info.sampleShadingEnable = VK_FALSE;
        multisample_state_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisample_state_create_info.minSampleShading = 1.0f;           // Optional
        multisample_state_create_info.pSampleMask = nullptr;             // Optional
        multisample_state_create_info.alphaToCoverageEnable = VK_FALSE;  // Optional
        multisample_state_create_info.alphaToOneEnable = VK_FALSE;       // Optional

        // Color blending
        VkPipelineColorBlendAttachmentState color_blend_attachment_state{};
        color_blend_attachment_state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                                      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        color_blend_attachment_state.blendEnable = VK_FALSE;
        color_blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
        color_blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
        color_blend_attachment_state.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
        color_blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
        color_blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
        color_blend_attachment_state.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

        // Computation:
        //     finalColor.rgb = newAlpha * newColor + (1 - newAlpha) * oldColor;
        //     finalColor.a = newAlpha.a;
        color_blend_attachment_state.blendEnable = VK_TRUE;
        color_blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        color_blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        color_blend_attachment_state.colorBlendOp = VK_BLEND_OP_ADD;
        color_blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        color_blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        color_blend_attachment_state.alphaBlendOp = VK_BLEND_OP_ADD;


        VkPipelineColorBlendStateCreateInfo color_blend_state_create_info{};
        color_blend_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        color_blend_state_create_info.logicOpEnable = VK_FALSE;
        color_blend_state_create_info.logicOp = VK_LOGIC_OP_COPY;  // Optional
        color_blend_state_create_info.attachmentCount = 1;
        color_blend_state_create_info.pAttachments = &color_blend_attachment_state;
        color_blend_state_create_info.blendConstants[0] = 0.0f;  // Optional
        color_blend_state_create_info.blendConstants[1] = 0.0f;  // Optional
        color_blend_state_create_info.blendConstants[2] = 0.0f;  // Optional
        color_blend_state_create_info.blendConstants[3] = 0.0f;  // Optional

        // Pipeline layout
        vulkan::PipelineLayout::CreateInfo pipeline_layout_create_info{};

        (*pipeline_layout_create_info).setLayoutCount = 0;             // Optional
        (*pipeline_layout_create_info).pSetLayouts = nullptr;          // Optional
        (*pipeline_layout_create_info).pushConstantRangeCount = 0;     // Optional
        (*pipeline_layout_create_info).pPushConstantRanges = nullptr;  // Optional

        _pipeline_layout = _device.pipeline_layout(pipeline_layout_create_info);

        // Graphics pipeline
        vulkan::Pipeline::GraphicsPipelineCreateInfo graphics_pipeline_create_info{};

        (*graphics_pipeline_create_info).stageCount = 2;
        (*graphics_pipeline_create_info).pStages = shader_stages;

        (*graphics_pipeline_create_info).pVertexInputState = vertex_input_state_create_info;
        (*graphics_pipeline_create_info).pInputAssemblyState = input_assembly_state_create_info;
        (*graphics_pipeline_create_info).pViewportState = &viewport_state_create_info;
        (*graphics_pipeline_create_info).pRasterizationState = &rasterization_state_create_info;
        (*graphics_pipeline_create_info).pMultisampleState = &multisample_state_create_info;
        (*graphics_pipeline_create_info).pDepthStencilState = nullptr;  // Optional
        (*graphics_pipeline_create_info).pColorBlendState = &color_blend_state_create_info;
        (*graphics_pipeline_create_info).pDynamicState = &dynamic_state_create_info;

        (*graphics_pipeline_create_info).layout = _pipeline_layout;
        (*graphics_pipeline_create_info).renderPass = _render_pass;
        (*graphics_pipeline_create_info).subpass = 0;

        (*graphics_pipeline_create_info).basePipelineHandle = VK_NULL_HANDLE;  // Optional
        (*graphics_pipeline_create_info).basePipelineIndex = -1;               // Optional

        _graphics_pipeline = _device.graphics_pipeline(graphics_pipeline_create_info);
    }


    void VulkanApplication::create_framebuffers()
    {
        _framebuffers.resize(_swapchain_image_views.size());

        for (std::size_t i = 0; i < _swapchain_image_views.size(); ++i)
        {
            VkImageView attachments[] = {_swapchain_image_views[i]};

            vulkan::Framebuffer::CreateInfo create_info{};
            (*create_info).renderPass = _render_pass;
            (*create_info).attachmentCount = 1;
            (*create_info).pAttachments = attachments;
            (*create_info).width = _image_extent.width;
            (*create_info).height = _image_extent.height;
            (*create_info).layers = 1;

            _framebuffers[i] = _device.framebuffer(create_info);
        }
    }


    void VulkanApplication::create_command_pool()
    {
        vulkan::QueueFamilies const queue_families{find_queue_families(_physical_device)};

        vulkan::CommandPool::CreateInfo create_info{};

        (*create_info).flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        (*create_info).queueFamilyIndex = queue_families.graphics_family();

        _command_pool = _device.command_pool(create_info);
    }


    void VulkanApplication::create_command_buffer()
    {
        vulkan::CommandBuffer::AllocateInfo allocate_info{};

        (*allocate_info).commandPool = _command_pool;
        (*allocate_info).level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        (*allocate_info).commandBufferCount = 1;

        _command_buffer = _device.command_buffer(allocate_info);
    }


    void VulkanApplication::create_sync_objects()
    {
        _image_available_semaphore = _device.semaphore(vulkan::Semaphore::CreateInfo{});
        _render_finished_semaphore = _device.semaphore(vulkan::Semaphore::CreateInfo{});

        vulkan::Fence::CreateInfo create_info{};

        // Create the fence in the signaled state, so the first first wait in draw_frame
        // returns immediately
        (*create_info).flags = VK_FENCE_CREATE_SIGNALED_BIT;

        _in_flight_fence = _device.fence(create_info);
    }


    void VulkanApplication::record_command_buffer(
        vulkan::CommandBuffer& command_buffer, std::uint32_t image_idx)
    {
        vulkan::CommandBuffer::BeginInfo command_buffer_begin_info{};
        (*command_buffer_begin_info).flags = 0;                   // Optional
        (*command_buffer_begin_info).pInheritanceInfo = nullptr;  // Optional

        command_buffer.begin(command_buffer_begin_info);

        // Render pass
        vulkan::RenderPass::BeginInfo render_pass_begin_info{};

        (*render_pass_begin_info).renderPass = _render_pass;
        (*render_pass_begin_info).framebuffer = _framebuffers[image_idx];

        (*render_pass_begin_info).renderArea.offset = {0, 0};
        (*render_pass_begin_info).renderArea.extent = _image_extent;

        VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        (*render_pass_begin_info).clearValueCount = 1;
        (*render_pass_begin_info).pClearValues = &clearColor;

        command_buffer.begin_render_pass(render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

        // Basic drawing commands
        command_buffer.bind_pipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, _graphics_pipeline);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(_image_extent.width);
        viewport.height = static_cast<float>(_image_extent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        command_buffer.set_viewport(&viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = _image_extent;

        command_buffer.set_scissor(&scissor);

        // Draw the triangle
        command_buffer.draw(3, 1, 0, 0);

        command_buffer.end_render_pass();
        command_buffer.end();
    }


    void VulkanApplication::main_loop()
    {
        bool quit = _window->should_close();

        while (!quit)
        {
            glfw::Library::poll_events();

            // imgui::glfw::VulkanFrame frame{*_window};

            // static Configuration configuration{};
            // quit = show_main_menu_bar(configuration);

            // if (!quit)
            // {
            //     show_datasets(datasets_to_visualize, configuration.show_details());
            // }

            draw_frame();

            quit = quit || _window->should_close();
        }

        _device.wait_idle();
    }


    void VulkanApplication::draw_frame()
    {
        // Wait for previous frame
        vkWaitForFences(_device, 1, _in_flight_fence, VK_TRUE, UINT64_MAX);
        vkResetFences(_device, 1, _in_flight_fence);

        // Acquire image from the swapchain
        uint32_t imageIndex;
        vkAcquireNextImageKHR(
            _device, _swapchain, UINT64_MAX, _image_available_semaphore, VK_NULL_HANDLE, &imageIndex);

        // Record the command buffer
        vkResetCommandBuffer(_command_buffer, 0);
        record_command_buffer(_command_buffer, imageIndex);

        // Submit the command buffer
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {_image_available_semaphore};

        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = _command_buffer;

        VkSemaphore signalSemaphores[] = {_render_finished_semaphore};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(_graphics_queue, 1, &submitInfo, _in_flight_fence) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        // Presentation
        vulkan::Device::PresentInfo present_info{};

        (*present_info).waitSemaphoreCount = 1;
        (*present_info).pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {_swapchain};
        (*present_info).swapchainCount = 1;
        (*present_info).pSwapchains = swapChains;

        (*present_info).pImageIndices = &imageIndex;
        (*present_info).pResults = nullptr;  // Optional

        _present_queue.present(present_info);
    }


    int VulkanApplication::run_implementation()
    {
        // TODO Make sure that creating a window is optional. We also want to support off-screen
        //      rendering. The imgui stuff is only used to interact with the visualizations,
        //      and to gain insight into the contents of data sets.
        //      - init_window (optional)
        //      - init_vulkan
        //      - init_imgui (optional)
        //      - enter main loop, and either(?)
        //          - allow user interaction, using glfw / imgui / vulkan stuff, ...
        //          - render and save, using only vulkan stuff
        //      - command line interface must make it easy to switch between these modes
        //      - To keep things separated, this function can call different functions, depending
        //        on the mode

        // TODO Move this elsewhere
        auto const dataset_names = argument<std::vector<std::string>>("<dataset>");

        init_window();
        init_vulkan();
        init_imgui();
        main_loop();

        return EXIT_SUCCESS;
    }

}  // namespace lue::view


/// static std::tuple<vulkan::PhysicalDevice, vulkan::QueueFamilies,
/// vulkan::PhysicalDevice::SurfaceProperties> select_physical_device(
///     vulkan::PhysicalDevices&& devices, vulkan::Surface const& surface,
///     vulkan::Names const& extension_names)
/// {
///     std::multimap<int, vulkan::PhysicalDevice> candidates{};

///     for (auto& device : devices)
///     {
///         int const score{rate_physical_device(device)};

///         candidates.insert(std::make_pair(score, std::move(device)));
///     }

///     auto it = candidates.rend();
///     vulkan::QueueFamilies queue_families{};
///     vulkan::PhysicalDevice::SurfaceProperties surface_properties{};

///     for (it = candidates.rbegin(); it != candidates.rend(); ++it)
///     {
///         if (it->first == 0)
///         {
///             // Candidates are ordered by score. We now reached the ones that are not
///             // suitable by definition.
///             it = candidates.rend();
///         }
///         else
///         {
///             // Check whether the device is suitable
///             queue_families = find_queue_families(it->second, surface);

///             bool const extensions_supported = it->second.extensions_available(extension_names);

///             bool swap_chain_is_adequate{false};

///             if(extensions_supported)
///             {
///                 surface_properties = it->second.surface_properties(surface);
///                 swap_chain_is_adequate =
///                    !surface_properties.formats().empty() &&
///                    !surface_properties.present_modes().empty();
///             }

///             if(queue_families.is_complete() && extensions_supported && swap_chain_is_adequate)
///             {
///                 break;
///             }
///         }
///     }

///     if (it == candidates.rend())
///     {
///         throw std::runtime_error("Failed to find a suitable GPU");
///     }

///     if (!queue_families.is_complete())
///     {
///         throw std::runtime_error("Failed to find a GPU supporting both graphics and
///         presentation");
///     }

///     return {std::move(candidates.rbegin()->second), queue_families,
///     std::move(surface_properties)};
/// }


/// static int rate_physical_device(vulkan::PhysicalDevice const& device)
/// {
///     int score{0};

///     vulkan::PhysicalDevice::Properties const properties{device.properties()};

///     // TODO Figure out which devices the expect here and whether the ranking here
///     //      is correct
///     // TODO Figure out which devices are actually connected to a screen. This is (only)
///     //      relevant when we need to present something on the screen.
///     if (properties.device_type() == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
///     {
///         score += 1000;
///     }
///     else if (properties.device_type() == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
///     {
///         score += 100;
///     }
///     else if (properties.device_type() == VK_PHYSICAL_DEVICE_TYPE_CPU)
///     {
///         score += 10;
///     }

///     return score;
/// }


/// static vulkan::QueueFamilies find_queue_families(
///     vulkan::PhysicalDevice const& physical_device, vulkan::Surface const& surface)
/// {
///     vulkan::QueueFamilyProperties queue_family_properties{physical_device.queue_family_properties()};
///     vulkan::QueueFamilies queue_families{};

///     for (std::uint32_t i = 0; i < queue_family_properties.size(); ++i)
///     {
///         // TODO What if multiple families support graphics or presentation? Currently
///         //      an assertion will trigger in the QueueFamilies member functions.

///         if (queue_family_properties[i].graphics())
///         {
///             queue_families.set_graphics_family(vulkan::QueueFamily{i});
///         }

///         if (physical_device.has_surface_support(vulkan::QueueFamily{i}, surface))
///         {
///             queue_families.set_present_family(vulkan::QueueFamily{i});
///         }
///     }

///     return queue_families;
/// }
