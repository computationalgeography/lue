#include "lue/view/vulkan_application.hpp"
#include "lue/configure.hpp"
#include "lue/glfw.hpp"
#include "lue/imgui.hpp"
#include <fmt/format.h>
#include <array>
#include <cassert>
#include <cmath>
#define GLM_FORCE_RADIANS
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <set>

#include <iostream>


namespace lue::view {
    namespace {

        struct Vertex
        {
                glm::vec2 pos;
                glm::vec3 color;

                static VkVertexInputBindingDescription binding_description()
                {
                    VkVertexInputBindingDescription description{};

                    description.binding = 0;
                    description.stride = sizeof(Vertex);
                    description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

                    return description;
                }

                static std::array<VkVertexInputAttributeDescription, 2> attribute_descriptions()
                {
                    std::array<VkVertexInputAttributeDescription, 2> descriptions{};

                    descriptions[0].binding = 0;
                    descriptions[0].location = 0;
                    descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;  // vec2: x, y
                    descriptions[0].offset = offsetof(Vertex, pos);

                    descriptions[1].binding = 0;
                    descriptions[1].location = 1;
                    descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;  // vec3: r, g, b
                    descriptions[1].offset = offsetof(Vertex, color);

                    return descriptions;
                }
        };


        std::vector<Vertex> const vertices = {
            {{-0.5f, -0.5f}, {1.0f, 0.0f, 1.0f}},  // 0: top left: red
            {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},   // 1: top right: green
            {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},    // 2: bottom right: blue
            {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}};  // 3: bottom left: white

        std::vector<std::uint16_t> indices = {0, 1, 2, 2, 3, 0};


        struct UniformBufferObject
        {
                alignas(16) glm::mat4 model;
                alignas(16) glm::mat4 view;
                alignas(16) glm::mat4 proj;
        };


        // This allows the CPU and GPU to work on their own tasks at the same time. If the CPU
        // finishes early, it will wait until the GPU finishes rendering before submitting
        // more work.
        std::uint32_t const MAX_NR_FRAMES_IN_FLIGHT = 2;

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


    void framebuffer_resize_callback(
        GLFWwindow* window, [[maybe_unused]] int width, [[maybe_unused]] int height)
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        auto application = reinterpret_cast<VulkanApplication*>(glfwGetWindowUserPointer(window));
        application->_framebuffer_resized = true;
    }


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
        _descriptor_set_layout{},
        _pipeline_layout{},
        _graphics_pipeline{},
        _framebuffers{},
        _command_pool{},
        _vertex_buffer{},
        _vertex_buffer_memory{},
        _index_buffer{},
        _index_buffer_memory{},
        _uniform_buffers{},
        _uniform_buffers_memory{},
        _uniform_buffers_mapped{},
        _descriptor_pool{},
        _descriptor_sets{},
        _command_buffers{},
        _image_available_semaphores{},
        _render_finished_semaphores{},
        _in_flight_fences{},
        _current_frame{0},
        _framebuffer_resized{false}

    {
#ifndef NDEBUG
        // Enable validation layers (only) in debug builds
        _enable_validation_layers = true;
#endif
    }


    VulkanApplication::~VulkanApplication()
    {
    }


    // void VulkanApplication::framebuffer_is_resized()
    // {
    //     _framebuffer_resized = true;
    // }


    void VulkanApplication::init_window()
    {
        // This stuff is needed to be able to draw to a window. When drawining to a file,
        // this should not be needed.

        _library = std::make_unique<glfw::Library>();
        _monitor = std::make_unique<glfw::Monitor>();

        assert(glfwVulkanSupported());

        // Create window with Vulkan context
        glfw::Window::hint(GLFW_CLIENT_API, GLFW_NO_API);
        // glfw::Window::hint(GLFW_RESIZABLE, GLFW_FALSE);  // TODO

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

        // Prevent the window from becoming smaller than some small size. Otherwise bad things
        // may happen (black screen seen on MESA-INTEL / Ivy bridge Vulkan / Intel(R) HD Graphics
        // 4000 (IVB GT2)).
        _window->set_size_limits(10, 10, GLFW_DONT_CARE, GLFW_DONT_CARE);
        _window->set_user_pointer(this);
        _window->set_framebuffer_size_callback(framebuffer_resize_callback);
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
        create_descriptor_set_layout();
        create_graphics_pipeline();
        create_framebuffers();
        create_command_pool();
        create_vertex_buffer();
        create_index_buffer();
        create_uniform_buffers();
        create_descriptor_pool();
        create_descriptor_sets();
        create_command_buffers();
        create_sync_objects();
    }


    void VulkanApplication::init_imgui()
    {
        // TODO

        // VkPipelineCache pipeline_cache{VK_NULL_HANDLE};

        // int const nr_images = _swapchain_images.size();

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

        std::vector<vulkan::Queue::CreateInfo> queue_create_infos(unique_queue_families.size());

        std::transform(
            unique_queue_families.begin(),
            unique_queue_families.end(),
            queue_create_infos.begin(),
            [](vulkan::QueueFamily const& queue_family)
            {
                static float const queue_priority{1.0f};
                vulkan::Queue::CreateInfo create_info{};

                (*create_info).queueFamilyIndex = queue_family;
                (*create_info).queueCount = 1;
                (*create_info).pQueuePriorities = &queue_priority;

                return create_info;
            });

        vulkan::Names required_extension_names{};
        required_extension_names.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        vulkan::Device::CreateInfo device_create_info{};

        // We are passing a pointer to the Vulkan structure that is layered in our Queue::CreateInfo
        // type. Internally, this pointer will be moved to reach the next info instance. This
        // should work when the Vulkan structure and our type have the same size. This is the case
        // when our structure does not have additional data members.
        static_assert(sizeof(vulkan::Queue::CreateInfo) == sizeof(vulkan::Queue::CreateInfo::VkType));
        (*device_create_info).pQueueCreateInfos = *queue_create_infos.data();
        (*device_create_info).queueCreateInfoCount = queue_create_infos.size();
        (*device_create_info).pEnabledFeatures = nullptr;
        (*device_create_info).enabledExtensionCount = required_extension_names.size();
        (*device_create_info).ppEnabledExtensionNames = required_extension_names.data();

        _device = vulkan::Device{_physical_device, device_create_info};

        // Possibly the same queue
        _graphics_queue = vulkan::Queue{_device.queue(queue_families.graphics_family())};
        _present_queue = vulkan::Queue{_device.queue(queue_families.present_family())};
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

        _swapchain = _device.create_swapchain(swapchain_create_info);

        _swapchain_images = _swapchain.images();
        _image_format = swap_surface_format.format;
        _image_extent = swap_extent;
    }


    void VulkanApplication::recreate_swapchain()
    {
        auto [width, height] = _window->framebuffer_size();

        while (width == 0 || height == 0)
        {
            // We are minimized. Wait until something interesting happens (resize?).
            glfwWaitEvents();
            std::tie(width, height) = _window->framebuffer_size();
        }

        // Don't touch resources that may still be in use
        _device.wait_idle();

        create_swapchain();
        create_image_views();
        create_framebuffers();
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

            _swapchain_image_views[i] = _device.create_image_view(create_info);
        }
    }


    void VulkanApplication::create_render_pass()
    {
        // Attachment description
        VkAttachmentDescription color_attachment{};
        color_attachment.format = _image_format;
        color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        // Subpasses and attachment references
        VkAttachmentReference color_attachment_ref{};
        color_attachment_ref.attachment = 0;
        color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &color_attachment_ref;

        // Subpass dependencies
        VkSubpassDependency subpass_dependency{};
        subpass_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        subpass_dependency.dstSubpass = 0;

        subpass_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        subpass_dependency.srcAccessMask = 0;
        subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        subpass_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        // Render pass
        vulkan::RenderPass::CreateInfo create_info{};
        (*create_info).attachmentCount = 1;
        (*create_info).pAttachments = &color_attachment;
        (*create_info).subpassCount = 1;
        (*create_info).pSubpasses = &subpass;
        (*create_info).dependencyCount = 1;
        (*create_info).pDependencies = &subpass_dependency;

        _render_pass = _device.create_render_pass(create_info);
    }


    void VulkanApplication::create_descriptor_set_layout()
    {
        vulkan::DescriptorSetLayout::Binding binding{};

        binding.binding = 0;
        binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        binding.descriptorCount = 1;
        binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        binding.pImmutableSamplers = nullptr;  // Optional

        vulkan::DescriptorSetLayout::CreateInfo create_info{};

        (*create_info).bindingCount = 1;
        (*create_info).pBindings = &binding;

        _descriptor_set_layout = _device.create_descriptor_set_layout(create_info);
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

        vulkan::ShaderModule vertex_shader_module{
            _device.create_shader_module(vertex_shader_module_create_info)};

        vulkan::ShaderModule::CreateInfo fragment_shader_module_create_info{};

        (*fragment_shader_module_create_info).codeSize = fragment_shader_code.size();
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        (*fragment_shader_module_create_info).pCode =
            reinterpret_cast<std::uint32_t const*>(fragment_shader_code.data());

        vulkan::ShaderModule fragment_shader_module{
            _device.create_shader_module(fragment_shader_module_create_info)};

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

        auto binding_description = Vertex::binding_description();
        auto attribute_descriptions = Vertex::attribute_descriptions();

        (*vertex_input_state_create_info).vertexBindingDescriptionCount = 1;
        (*vertex_input_state_create_info).pVertexBindingDescriptions = &binding_description;
        (*vertex_input_state_create_info).vertexAttributeDescriptionCount = attribute_descriptions.size();
        (*vertex_input_state_create_info).pVertexAttributeDescriptions = attribute_descriptions.data();

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

        vulkan::Pipeline::DynamicStateCreateInfo dynamic_state_create_info{};
        std::vector<VkDynamicState> dynamic_states = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
        (*dynamic_state_create_info).dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
        (*dynamic_state_create_info).pDynamicStates = dynamic_states.data();

        vulkan::Pipeline::ViewportStateCreateInfo viewport_state_create_info{};
        (*viewport_state_create_info).viewportCount = 1;
        (*viewport_state_create_info).scissorCount = 1;

        // Rasterizer
        VkPipelineRasterizationStateCreateInfo rasterization_state_create_info{};
        rasterization_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterization_state_create_info.depthClampEnable = VK_FALSE;
        rasterization_state_create_info.rasterizerDiscardEnable = VK_FALSE;
        rasterization_state_create_info.polygonMode = VK_POLYGON_MODE_FILL;
        rasterization_state_create_info.lineWidth = 1.0f;
        rasterization_state_create_info.cullMode = VK_CULL_MODE_BACK_BIT;

        // Because of the Y-flip in the projection matrix, the vertices are being drawn in
        // counter-clockwise order instead of clockwise order. This causes backface culling and
        // prevents any geometry from being drawn. This is to correct for that.
        rasterization_state_create_info.frontFace =
            VK_FRONT_FACE_COUNTER_CLOCKWISE;  // VK_FRONT_FACE_CLOCKWISE;
        rasterization_state_create_info.depthBiasEnable = VK_FALSE;
        rasterization_state_create_info.depthBiasConstantFactor = 0.0f;  // Optional
        rasterization_state_create_info.depthBiasClamp = 0.0f;           // Optional
        rasterization_state_create_info.depthBiasSlopeFactor = 0.0f;     // Optional

        // Multisampling
        vulkan::Pipeline::MultisampleStateCreateInfo multisample_state_create_info{};
        (*multisample_state_create_info).sampleShadingEnable = VK_FALSE;
        (*multisample_state_create_info).rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        (*multisample_state_create_info).minSampleShading = 1.0f;           // Optional
        (*multisample_state_create_info).pSampleMask = nullptr;             // Optional
        (*multisample_state_create_info).alphaToCoverageEnable = VK_FALSE;  // Optional
        (*multisample_state_create_info).alphaToOneEnable = VK_FALSE;       // Optional

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

        vulkan::Pipeline::ColorBlendStateCreateInfo color_blend_state_create_info{};
        (*color_blend_state_create_info).logicOpEnable = VK_FALSE;
        (*color_blend_state_create_info).logicOp = VK_LOGIC_OP_COPY;  // Optional
        (*color_blend_state_create_info).attachmentCount = 1;
        (*color_blend_state_create_info).pAttachments = &color_blend_attachment_state;
        (*color_blend_state_create_info).blendConstants[0] = 0.0f;  // Optional
        (*color_blend_state_create_info).blendConstants[1] = 0.0f;  // Optional
        (*color_blend_state_create_info).blendConstants[2] = 0.0f;  // Optional
        (*color_blend_state_create_info).blendConstants[3] = 0.0f;  // Optional

        // Pipeline layout
        vulkan::PipelineLayout::CreateInfo pipeline_layout_create_info{};

        (*pipeline_layout_create_info).setLayoutCount = 1;
        (*pipeline_layout_create_info).pSetLayouts = _descriptor_set_layout;
        (*pipeline_layout_create_info).pushConstantRangeCount = 0;     // Optional
        (*pipeline_layout_create_info).pPushConstantRanges = nullptr;  // Optional

        _pipeline_layout = _device.create_pipeline_layout(pipeline_layout_create_info);

        // Graphics pipeline
        vulkan::Pipeline::GraphicsPipelineCreateInfo graphics_pipeline_create_info{};

        (*graphics_pipeline_create_info).stageCount = 2;
        (*graphics_pipeline_create_info).pStages = shader_stages;

        (*graphics_pipeline_create_info).pVertexInputState = vertex_input_state_create_info;
        (*graphics_pipeline_create_info).pInputAssemblyState = input_assembly_state_create_info;
        (*graphics_pipeline_create_info).pViewportState = viewport_state_create_info;
        (*graphics_pipeline_create_info).pRasterizationState = &rasterization_state_create_info;
        (*graphics_pipeline_create_info).pMultisampleState = multisample_state_create_info;
        (*graphics_pipeline_create_info).pDepthStencilState = nullptr;  // Optional
        (*graphics_pipeline_create_info).pColorBlendState = color_blend_state_create_info;
        (*graphics_pipeline_create_info).pDynamicState = dynamic_state_create_info;

        (*graphics_pipeline_create_info).layout = _pipeline_layout;
        (*graphics_pipeline_create_info).renderPass = _render_pass;
        (*graphics_pipeline_create_info).subpass = 0;

        (*graphics_pipeline_create_info).basePipelineHandle = VK_NULL_HANDLE;  // Optional
        (*graphics_pipeline_create_info).basePipelineIndex = -1;               // Optional

        _graphics_pipeline = _device.create_graphics_pipeline(graphics_pipeline_create_info);
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

            _framebuffers[i] = _device.create_framebuffer(create_info);
        }
    }


    void VulkanApplication::create_command_pool()
    {
        vulkan::QueueFamilies const queue_families{find_queue_families(_physical_device)};

        vulkan::CommandPool::CreateInfo create_info{};

        (*create_info).flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        (*create_info).queueFamilyIndex = queue_families.graphics_family();

        _command_pool = _device.create_command_pool(create_info);
    }


    std::uint32_t VulkanApplication::find_memory_type(
        std::uint32_t const type_filter, VkMemoryPropertyFlags const properties) const
    {
        auto memory_properties{_physical_device.memory_properties()};

        for (std::uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i)
        {
            if ((type_filter & (1 << i)) &&
                (memory_properties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }

        throw std::runtime_error("Failed to find suitable memory type");
    }


    std::tuple<vulkan::Buffer, vulkan::Memory> VulkanApplication::create_buffer(
        VkDeviceSize const size, VkBufferUsageFlags const usage, VkMemoryPropertyFlags const properties)
    {
        vulkan::Buffer::CreateInfo create_info{};

        (*create_info).size = size;
        (*create_info).usage = usage;
        (*create_info).sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        auto buffer = _device.create_buffer(create_info);

        auto memory_requirements{buffer.memory_requirements()};

        vulkan::Memory::AllocateInfo allocate_info{};

        (*allocate_info).allocationSize = memory_requirements.size;
        (*allocate_info).memoryTypeIndex = find_memory_type(memory_requirements.memoryTypeBits, properties);

        auto vertex_buffer_memory = _device.allocate_memory(allocate_info);

        buffer.bind(vertex_buffer_memory, 0);

        return {std::move(buffer), std::move(vertex_buffer_memory)};
    }


    void VulkanApplication::copy_buffer(
        vulkan::Buffer& from_buffer, vulkan::Buffer& to_buffer, VkDeviceSize const size)
    {
        vulkan::CommandBuffer::AllocateInfo allocate_info{};

        (*allocate_info).commandPool = _command_pool;
        (*allocate_info).level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        (*allocate_info).commandBufferCount = 1;

        vulkan::CommandBuffer command_buffer = std::move(_device.allocate_command_buffers(allocate_info)[0]);

        vulkan::CommandBuffer::BeginInfo command_buffer_begin_info{};
        (*command_buffer_begin_info).flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        command_buffer.begin(command_buffer_begin_info);

        VkBufferCopy copy_region{};
        copy_region.srcOffset = 0;  // Optional
        copy_region.dstOffset = 0;  // Optional
        copy_region.size = size;
        vkCmdCopyBuffer(command_buffer, from_buffer, to_buffer, 1, &copy_region);

        command_buffer.end();

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = command_buffer;

        vkQueueSubmit(_graphics_queue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(_graphics_queue);

        vkFreeCommandBuffers(_device, _command_pool, 1, command_buffer);
    }


    void VulkanApplication::create_vertex_buffer()
    {
        VkDeviceSize const buffer_size = sizeof(vertices[0]) * vertices.size();

        auto [staging_buffer, staging_buffer_memory] = create_buffer(
            buffer_size,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        auto data = _device.map_memory(staging_buffer_memory, 0, buffer_size, 0);

        std::memcpy(data, vertices.data(), buffer_size);

        _device.unmap_memory(staging_buffer_memory);

        std::tie(_vertex_buffer, _vertex_buffer_memory) = create_buffer(
            buffer_size,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        copy_buffer(staging_buffer, _vertex_buffer, buffer_size);
    }


    void VulkanApplication::create_index_buffer()
    {
        VkDeviceSize const buffer_size = sizeof(indices[0]) * indices.size();

        auto [staging_buffer, staging_buffer_memory] = create_buffer(
            buffer_size,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        auto data = _device.map_memory(staging_buffer_memory, 0, buffer_size, 0);

        std::memcpy(data, indices.data(), buffer_size);

        _device.unmap_memory(staging_buffer_memory);

        std::tie(_index_buffer, _index_buffer_memory) = create_buffer(
            buffer_size,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        copy_buffer(staging_buffer, _index_buffer, buffer_size);
    }


    void VulkanApplication::create_uniform_buffers()
    {
        VkDeviceSize const buffer_size = sizeof(UniformBufferObject);

        _uniform_buffers.resize(MAX_NR_FRAMES_IN_FLIGHT);
        _uniform_buffers_memory.resize(MAX_NR_FRAMES_IN_FLIGHT);
        _uniform_buffers_mapped.resize(MAX_NR_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < MAX_NR_FRAMES_IN_FLIGHT; i++)
        {
            std::tie(_uniform_buffers[i], _uniform_buffers_memory[i]) = create_buffer(
                buffer_size,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

            _uniform_buffers_mapped[i] = _device.map_memory(_uniform_buffers_memory[i], 0, buffer_size, 0);
        }
    }


    void VulkanApplication::create_descriptor_pool()
    {
        vulkan::DescriptorPool::PoolSize pool_size{};

        pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        pool_size.descriptorCount = static_cast<uint32_t>(MAX_NR_FRAMES_IN_FLIGHT);

        vulkan::DescriptorPool::CreateInfo create_info{};

        (*create_info).poolSizeCount = 1;
        (*create_info).pPoolSizes = &pool_size;
        (*create_info).maxSets = static_cast<uint32_t>(MAX_NR_FRAMES_IN_FLIGHT);

        _descriptor_pool = _device.create_descriptor_pool(create_info);
    }


    void VulkanApplication::create_descriptor_sets()
    {
        std::vector<VkDescriptorSetLayout> layouts(MAX_NR_FRAMES_IN_FLIGHT, _descriptor_set_layout);

        vulkan::DescriptorSet::AllocateInfo allocate_info{};

        (*allocate_info).descriptorPool = _descriptor_pool;
        (*allocate_info).descriptorSetCount = MAX_NR_FRAMES_IN_FLIGHT;
        (*allocate_info).pSetLayouts = layouts.data();

        _descriptor_sets.resize(MAX_NR_FRAMES_IN_FLIGHT);

        if (vkAllocateDescriptorSets(_device, allocate_info, _descriptor_sets.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        for (size_t i = 0; i < MAX_NR_FRAMES_IN_FLIGHT; i++)
        {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = _uniform_buffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(UniformBufferObject);

            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = _descriptor_sets[i];
            descriptorWrite.dstBinding = 0;
            descriptorWrite.dstArrayElement = 0;

            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrite.descriptorCount = 1;

            descriptorWrite.pBufferInfo = &bufferInfo;
            descriptorWrite.pImageInfo = nullptr;        // Optional
            descriptorWrite.pTexelBufferView = nullptr;  // Optional

            vkUpdateDescriptorSets(_device, 1, &descriptorWrite, 0, nullptr);
        }
    }


    void VulkanApplication::create_command_buffers()
    {
        vulkan::CommandBuffer::AllocateInfo allocate_info{};

        (*allocate_info).commandPool = _command_pool;
        (*allocate_info).level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        (*allocate_info).commandBufferCount = MAX_NR_FRAMES_IN_FLIGHT;

        _command_buffers = _device.allocate_command_buffers(allocate_info);
    }


    void VulkanApplication::create_sync_objects()
    {
        _image_available_semaphores =
            _device.create_semaphores(MAX_NR_FRAMES_IN_FLIGHT, vulkan::Semaphore::CreateInfo{});
        _render_finished_semaphores =
            _device.create_semaphores(MAX_NR_FRAMES_IN_FLIGHT, vulkan::Semaphore::CreateInfo{});

        vulkan::Fence::CreateInfo create_info{};

        // Create the fence in the signaled state, so the first first wait in draw_frame
        // returns immediately
        (*create_info).flags = VK_FENCE_CREATE_SIGNALED_BIT;

        _in_flight_fences = _device.create_fences(MAX_NR_FRAMES_IN_FLIGHT, create_info);
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
        command_buffer.bind_vertex_buffer(_vertex_buffer);
        command_buffer.bind_index_buffer(_index_buffer, VK_INDEX_TYPE_UINT16);

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
        // command_buffer.draw(static_cast<std::uint32_t>(vertices.size()), 1, 0, 0);
        vkCmdBindDescriptorSets(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            _pipeline_layout,
            0,
            1,
            &_descriptor_sets[_current_frame],
            0,
            nullptr);
        // command_buffer.bind_descriptor_sets(VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline_layout, 0, 1,
        // &_descriptor_sets[_current_frame], 0, nullptr);
        command_buffer.draw_indexed(static_cast<std::uint32_t>(indices.size()), 1, 0, 0, 0);

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


    void VulkanApplication::update_uniform_buffer(std::uint32_t const current_frame)
    {
        static auto const start_time = std::chrono::high_resolution_clock::now();

        auto current_time = std::chrono::high_resolution_clock::now();
        float time =
            std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();

        UniformBufferObject ubo{};

        // Model
        ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

        // View
        ubo.view = glm::lookAt(
            glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

        // Projection
        ubo.proj = glm::perspective(
            glm::radians(45.0f), _image_extent.width / (float)_image_extent.height, 0.1f, 10.0f);

        // glm assumes OpenGL. Correct for that.
        ubo.proj[1][1] *= -1;

        std::memcpy(_uniform_buffers_mapped[_current_frame], &ubo, sizeof(ubo));
    }


    void VulkanApplication::draw_frame()
    {
        // Wait for previous frame
        _device.wait(_in_flight_fences[_current_frame]);

        // Acquire image from the swapchain
        auto [result, image_idx] =
            _device.acquire_next_image(_swapchain, _image_available_semaphores[_current_frame]);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            recreate_swapchain();
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("Failed to acquire swapchain image");
        }

        // Only reset the fence if we are submitting work
        _device.reset(_in_flight_fences[_current_frame]);

        update_uniform_buffer(_current_frame);

        // Record the command buffer
        vkResetCommandBuffer(_command_buffers[_current_frame], 0);
        record_command_buffer(_command_buffers[_current_frame], image_idx);

        // Submit the command buffer
        VkSubmitInfo submit_info{};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {_image_available_semaphores[_current_frame]};

        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = waitSemaphores;
        submit_info.pWaitDstStageMask = waitStages;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = _command_buffers[_current_frame];

        VkSemaphore signalSemaphores[] = {_render_finished_semaphores[_current_frame]};
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(_graphics_queue, 1, &submit_info, _in_flight_fences[_current_frame]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        // Presentation
        vulkan::Queue::PresentInfo present_info{};

        (*present_info).waitSemaphoreCount = 1;
        (*present_info).pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {_swapchain};
        (*present_info).swapchainCount = 1;
        (*present_info).pSwapchains = swapChains;

        (*present_info).pImageIndices = &image_idx;
        (*present_info).pResults = nullptr;  // Optional

        result = _present_queue.present(present_info);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _framebuffer_resized)
        {
            _framebuffer_resized = false;
            recreate_swapchain();
        }
        else if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to present swapchain image");
        }

        _current_frame = (_current_frame + 1) % MAX_NR_FRAMES_IN_FLIGHT;
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
