#include "lue/view/vulkan_application.hpp"
#include "lue/configure.hpp"
#include "lue/glfw.hpp"
#include "lue/imgui.hpp"
#include "lue/vulkan.hpp"
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


        static vulkan::Surface create_surface(vulkan::Instance const& instance, glfw::Window& window)
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
        _instance = vulkan::Instance{vulkan::Instance::CreateInfo{
            vulkan::ApplicationInfo{
                application_name,
                std::make_tuple(
                    BuildOptions::major_version, BuildOptions::minor_version, BuildOptions::patch_version),
                VK_API_VERSION_1_2},
            required_layer_names(),
            required_extension_names()}};
    }


    void VulkanApplication::setup_debug_messenger()
    {
        _debug_callback = vulkan::DebugReportCallback{
            _instance,
            vulkan::DebugReportCallback::CreateInfo{
                VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT |
                    VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT,
                debug_report}};
    }


    void VulkanApplication::create_surface()
    {
        // Create surface, right after creating the instance and configuring the debugging
        // messenger. Otherwise the other code may behave differently, given that we created
        // a surface.

        _surface = vulkan::Surface{lue::view::create_surface(_instance, *_window)};
    }


    vulkan::QueueFamilies VulkanApplication::find_queue_families(
        vulkan::PhysicalDevice const& physical_device) const
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


    bool VulkanApplication::physical_device_is_suitable(vulkan::PhysicalDevice const& device) const
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
            { return vulkan::Device::QueueCreateInfo{queue_family}; });

        vulkan::Names required_extension_names{};
        required_extension_names.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        _device = vulkan::Device{
            _physical_device,
            vulkan::Device::CreateInfo{std::move(queue_create_infos), std::move(required_extension_names)}};

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

        _swapchain = _device.swapchain(vulkan::Swapchain::CreateInfo{
            _surface, nr_images, swap_surface_format, swap_extent, queue_families, transform, present_mode});
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

        vulkan::ShaderModule vertex_shader_module{
            _device.shader_module(vulkan::ShaderModule::CreateInfo{vertex_shader_code})};
        vulkan::ShaderModule fragment_shader_module{
            _device.shader_module(vulkan::ShaderModule::CreateInfo{fragment_shader_code})};

        vulkan::Pipeline::ShaderStageCreateInfo vertex_shader_stage_info{
            VK_SHADER_STAGE_VERTEX_BIT, vertex_shader_module};
        vulkan::Pipeline::ShaderStageCreateInfo fragment_shader_stage_info{
            VK_SHADER_STAGE_FRAGMENT_BIT, fragment_shader_module};

        VkPipelineShaderStageCreateInfo shader_stages[] = {
            *vertex_shader_stage_info, *fragment_shader_stage_info};

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 0;
        vertexInputInfo.pVertexBindingDescriptions = nullptr;  // Optional
        vertexInputInfo.vertexAttributeDescriptionCount = 0;
        vertexInputInfo.pVertexAttributeDescriptions = nullptr;  // Optional

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

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

        std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();


        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;


        // Rasterizer
        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f;  // Optional
        rasterizer.depthBiasClamp = 0.0f;           // Optional
        rasterizer.depthBiasSlopeFactor = 0.0f;     // Optional

        // Multisampling
        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading = 1.0f;           // Optional
        multisampling.pSampleMask = nullptr;             // Optional
        multisampling.alphaToCoverageEnable = VK_FALSE;  // Optional
        multisampling.alphaToOneEnable = VK_FALSE;       // Optional

        // Color blending
        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                              VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

        // Computation:
        //     finalColor.rgb = newAlpha * newColor + (1 - newAlpha) * oldColor;
        //     finalColor.a = newAlpha.a;
        colorBlendAttachment.blendEnable = VK_TRUE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;  // Optional
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;  // Optional
        colorBlending.blendConstants[1] = 0.0f;  // Optional
        colorBlending.blendConstants[2] = 0.0f;  // Optional
        colorBlending.blendConstants[3] = 0.0f;  // Optional


        // Pipeline layout
        _pipeline_layout = _device.pipeline_layout(vulkan::PipelineLayout::CreateInfo{});


        vulkan::Pipeline::GraphicsPipelineCreateInfo create_info{};

        (*create_info).stageCount = 2;
        (*create_info).pStages = shader_stages;

        (*create_info).pVertexInputState = &vertexInputInfo;
        (*create_info).pInputAssemblyState = &inputAssembly;
        (*create_info).pViewportState = &viewportState;
        (*create_info).pRasterizationState = &rasterizer;
        (*create_info).pMultisampleState = &multisampling;
        (*create_info).pDepthStencilState = nullptr;  // Optional
        (*create_info).pColorBlendState = &colorBlending;
        (*create_info).pDynamicState = &dynamicState;

        (*create_info).layout = _pipeline_layout;
        (*create_info).renderPass = _render_pass;
        (*create_info).subpass = 0;

        (*create_info).basePipelineHandle = VK_NULL_HANDLE;  // Optional
        (*create_info).basePipelineIndex = -1;               // Optional

        _graphics_pipeline = _device.graphics_pipeline(create_info);
    }


    void VulkanApplication::create_framebuffers()
    {
        _framebuffers.resize(_swapchain_image_views.size());

        for (std::size_t i = 0; i < _swapchain_image_views.size(); ++i)
        {
            VkImageView attachments[] = {_swapchain_image_views[i]};

            vulkan::Framebuffer::CreateInfo create_info{};
            (*create_info).renderPass = _render_pass;
            (*create_info).attachmentCount = _render_pass;
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
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0;                   // Optional
        beginInfo.pInheritanceInfo = nullptr;  // Optional

        if (vkBeginCommandBuffer(command_buffer, &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer!");
        }


        // Render pass
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = _render_pass;
        renderPassInfo.framebuffer = _framebuffers[image_idx];


        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = _image_extent;

        VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;


        // TODO Make this a member of CommandBuffer
        vkCmdBeginRenderPass(command_buffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);


        // Basic drawing commands
        vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _graphics_pipeline);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(_image_extent.width);
        viewport.height = static_cast<float>(_image_extent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(command_buffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = _image_extent;
        vkCmdSetScissor(command_buffer, 0, 1, &scissor);

        // Draw the rectangle
        vkCmdDraw(command_buffer, 3, 1, 0, 0);


        vkCmdEndRenderPass(command_buffer);


        if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer!");
        }
    }


    void VulkanApplication::main_loop()
    {
        while (!_window->should_close())
        {
            glfw::Library::poll_events();
            draw_frame();
        }

        vkDeviceWaitIdle(_device);
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
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {_swapchain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.pResults = nullptr;  // Optional

        vkQueuePresentKHR(_present_queue, &presentInfo);
    }


    int VulkanApplication::run_implementation()
    {


        // TODO Make sure that creating a window is optional. We also want to support off-screen
        //      rendering. The imgui stuff is only used to interact with the visualizations,
        //      and to gain insight into the contents of data sets.
        //      - init_window (optional)
        //      - init_vulkan
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
        main_loop();

        return EXIT_SUCCESS;


        // PRESENTATION


        //         // Create Framebuffers
        //         int w, h;
        //         glfwGetFramebufferSize(glfw_window, &w, &h);
        //
        //
        //         ImGui_ImplVulkanH_Window* wd = &g_MainWindowData;
        //         SetupVulkanWindow(wd, surface, w, h);


        // Vulkan_Init
        // imgui::glfw::VulkanBinding binding{glfw_window};

        //         ImGui_ImplVulkan_InitInfo init_info = {};
        //         init_info.Instance = g_Instance;
        //         init_info.PhysicalDevice = g_PhysicalDevice;
        //         init_info.Device = g_Device;
        //         init_info.QueueFamily = g_QueueFamily;
        //         init_info.Queue = g_Queue;
        //         init_info.PipelineCache = g_PipelineCache;
        //         init_info.DescriptorPool = g_DescriptorPool;
        //         init_info.Subpass = 0;
        //         init_info.MinImageCount = g_MinImageCount;
        //         init_info.ImageCount = wd->ImageCount;
        //         init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        //         init_info.Allocator = g_Allocator;
        //         init_info.CheckVkResultFn = check_vk_result;
        //         ImGui_ImplVulkan_Init(&init_info, wd->RenderPass);


        //         // Load Fonts
        //         // - If no fonts are loaded, dear imgui will use the default font. You can also load
        //         multiple fonts and use ImGui::PushFont()/PopFont() to select them.
        //         // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select
        //         the font among multiple.
        //         // - If the file cannot be loaded, the function will return a nullptr. Please handle those
        //         errors in your application (e.g. use an assertion, or display an error and quit).
        //         // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a
        //         texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame
        //         below will call.
        //         // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher
        //         quality font rendering.
        //         // - Read 'docs/FONTS.md' for more instructions and details.
        //         // - Remember that in C/C++ if you want to include a backslash \ in a string literal you
        //         need to write a double backslash \\ !
        //         //io.Fonts->AddFontDefault();
        //         //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
        //         //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
        //         //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
        //         //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
        //         //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f,
        //         nullptr, io.Fonts->GetGlyphRangesJapanese());
        //         //IM_ASSERT(font != nullptr);
        //
        //         // Upload Fonts
        //         {
        //             // Use any command queue
        //             VkCommandPool command_pool = wd->Frames[wd->FrameIndex].CommandPool;
        //             VkCommandBuffer command_buffer = wd->Frames[wd->FrameIndex].CommandBuffer;
        //
        //             err = vkResetCommandPool(g_Device, command_pool, 0);
        //             check_vk_result(err);
        //             VkCommandBufferBeginInfo begin_info = {};
        //             begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        //             begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        //             err = vkBeginCommandBuffer(command_buffer, &begin_info);
        //             check_vk_result(err);
        //
        //             ImGui_ImplVulkan_CreateFontsTexture(command_buffer);
        //
        //             VkSubmitInfo end_info = {};
        //             end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        //             end_info.commandBufferCount = 1;
        //             end_info.pCommandBuffers = &command_buffer;
        //             err = vkEndCommandBuffer(command_buffer);
        //             check_vk_result(err);
        //             err = vkQueueSubmit(g_Queue, 1, &end_info, VK_NULL_HANDLE);
        //             check_vk_result(err);
        //
        //             err = vkDeviceWaitIdle(g_Device);
        //             check_vk_result(err);
        //             ImGui_ImplVulkan_DestroyFontUploadObjects();
        //         }


        //         // Our state
        //         bool show_demo_window = true;
        //         bool show_another_window = false;
        //         ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


        // DRAWING


        // Main loop
        bool quit = _window->should_close();

        while (!quit)
        {
            glfw::Library::poll_events();

            // imgui::glfw::VulkanFrame frame{glfw_window};

            // static Configuration configuration{};
            // quit = show_main_menu_bar(configuration);

            // if (!quit)
            // {
            //     show_datasets(datasets_to_visualize, configuration.show_details());
            // }

            quit = quit || _window->should_close();


            //             // Resize swap chain?
            //             if (g_SwapChainRebuild)
            //             {
            //                 int width, height;
            //                 glfwGetFramebufferSize(glfw_window, &width, &height);
            //                 if (width > 0 && height > 0)
            //                 {
            //                     ImGui_ImplVulkan_SetMinImageCount(g_MinImageCount);
            //                     ImGui_ImplVulkanH_CreateOrResizeWindow(g_Instance, g_PhysicalDevice,
            //                     g_Device, &g_MainWindowData, g_QueueFamily, g_Allocator, width, height,
            //                     g_MinImageCount); g_MainWindowData.FrameIndex = 0; g_SwapChainRebuild =
            //                     false;
            //                 }
            //             }
            //
            //             // Start the Dear ImGui frame
            //             ImGui_ImplVulkan_NewFrame();
            //             ImGui_ImplGlfw_NewFrame();
            //             ImGui::NewFrame();
            //
            //             // 1. Show the big demo window (Most of the sample code is in
            //             ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
            //             if (show_demo_window)
            //                 ImGui::ShowDemoWindow(&show_demo_window);
            //
            //             // 2. Show a simple window that we create ourselves. We use a Begin/End pair to
            //             create a named window.
            //             {
            //                 static float f = 0.0f;
            //                 static int counter = 0;
            //
            //                 ImGui::Begin("Hello, world!");                          // Create a window
            //                 called "Hello, world!" and append into it.
            //
            //                 ImGui::Text("This is some useful text.");               // Display some text
            //                 (you can use a format strings too) ImGui::Checkbox("Demo Window",
            //                 &show_demo_window);      // Edit bools storing our window open/close state
            //                 ImGui::Checkbox("Another Window", &show_another_window);
            //
            //                 ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a
            //                 slider from 0.0f to 1.0f ImGui::ColorEdit3("clear color",
            //                 (float*)&clear_color); // Edit 3 floats representing a color
            //
            //                 if (ImGui::Button("Button"))                            // Buttons return true
            //                 when clicked (most widgets return true when edited/activated)
            //                     counter++;
            //                 ImGui::SameLine();
            //                 ImGui::Text("counter = %d", counter);
            //
            //                 ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f /
            //                 imgui::glfw::VulkanBinding::io().Framerate,
            //                 imgui::glfw::VulkanBinding::io().Framerate); ImGui::End();
            //             }
            //
            //             // 3. Show another simple window.
            //             if (show_another_window)
            //             {
            //                 ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to
            //                 our bool variable (the window will have a closing button that will clear the
            //                 bool when clicked) ImGui::Text("Hello from another window!"); if
            //                 (ImGui::Button("Close Me"))
            //                     show_another_window = false;
            //                 ImGui::End();
            //             }
            //
            //             // Rendering
            //             ImGui::Render();
            //             ImDrawData* draw_data = ImGui::GetDrawData();
            //             const bool is_minimized = (draw_data->DisplaySize.x <= 0.0f ||
            //             draw_data->DisplaySize.y <= 0.0f); if (!is_minimized)
            //             {
            //                 wd->ClearValue.color.float32[0] = clear_color.x * clear_color.w;
            //                 wd->ClearValue.color.float32[1] = clear_color.y * clear_color.w;
            //                 wd->ClearValue.color.float32[2] = clear_color.z * clear_color.w;
            //                 wd->ClearValue.color.float32[3] = clear_color.w;
            //                 FrameRender(wd, draw_data);
            //                 FramePresent(wd);
            //             }
        }


        //         // Cleanup
        //         err = vkDeviceWaitIdle(g_Device);
        //         check_vk_result(err);
        //         // ImGui_ImplVulkan_Shutdown();
        //         // ImGui_ImplGlfw_Shutdown();
        //         ImGui::DestroyContext();
        //
        //         CleanupVulkanWindow();
        //         CleanupVulkan();
        //
        //         // glfwDestroyWindow(glfw_window);
        //         // glfwTerminate();
        //
        //
        //
        //
        //
        //
        //         // bool quit = glfw_window.should_close();
        //
        //         // while (!quit)
        //         // {
        //         //     glfwPollEvents();
        //
        //         //     imgui::glfw::VulkanFrame frame{glfw_window};
        //
        //         //     static Configuration configuration{};
        //         //     quit = show_main_menu_bar(configuration);
        //
        //         //     // if (!quit)
        //         //     // {
        //         //     //     show_datasets(datasets_to_visualize, configuration.show_details());
        //         //     // }
        //
        //
        //
        //
        //
        //         //     quit = quit || glfw_window.should_close();
        //         // }

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
