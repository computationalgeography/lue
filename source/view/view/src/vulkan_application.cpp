#include "lue/view/vulkan_application.hpp"
#include "lue/configure.hpp"
#include "lue/glfw.hpp"
#include "lue/imgui.hpp"
#include "lue/vulkan.hpp"
#include <fmt/format.h>
#include <cassert>
#include <cmath>

#include <iostream>


// #include "imgui_impl_glfw.h"
// #include "imgui_impl_vulkan.h"
//
//
//
//
//
// #ifdef _DEBUG
// #define IMGUI_VULKAN_DEBUG_REPORT
// #endif
//
// // Data
// static VkAllocationCallbacks*   g_Allocator = nullptr;
// static VkInstance               g_Instance = VK_NULL_HANDLE;
// static VkPhysicalDevice         g_PhysicalDevice = VK_NULL_HANDLE;
// static VkDevice                 g_Device = VK_NULL_HANDLE;
// static uint32_t                 g_QueueFamily = (uint32_t)-1;
// static VkQueue                  g_Queue = VK_NULL_HANDLE;
// static VkDebugReportCallbackEXT g_DebugReport = VK_NULL_HANDLE;
// static VkPipelineCache          g_PipelineCache = VK_NULL_HANDLE;
// static VkDescriptorPool         g_DescriptorPool = VK_NULL_HANDLE;
//
// static ImGui_ImplVulkanH_Window g_MainWindowData;
// static int                      g_MinImageCount = 2;
// static bool                     g_SwapChainRebuild = false;
//
// static void glfw_error_callback(int error, const char* description)
// {
//     fprintf(stderr, "GLFW Error %d: %s\n", error, description);
// }

// static void check_vk_result(VkResult err)
// {
//     if (err == 0)
//         return;
//     fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
//     if (err < 0)
//         abort();
// }


#ifndef NDEBUG
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
    std::cerr << fmt::format("[vulkan] Debug report from ObjectType: {}: Message: {}", object_type, message)
              << std::endl;

    return VK_FALSE;
}
#endif


// static bool IsExtensionAvailable(const ImVector<VkExtensionProperties>& properties, const char* extension)
// {
//     for (const VkExtensionProperties& p : properties)
//         if (strcmp(p.extensionName, extension) == 0)
//             return true;
//     return false;
// }
//
//
// static VkPhysicalDevice SetupVulkan_SelectPhysicalDevice()
// {
//     uint32_t gpu_count;
//     VkResult err = vkEnumeratePhysicalDevices(g_Instance, &gpu_count, nullptr);
//     check_vk_result(err);
//     IM_ASSERT(gpu_count > 0);
//
//     ImVector<VkPhysicalDevice> gpus;
//     gpus.resize(gpu_count);
//     err = vkEnumeratePhysicalDevices(g_Instance, &gpu_count, gpus.Data);
//     check_vk_result(err);
//
//     // If a number >1 of GPUs got reported, find discrete GPU if present, or use first one available. This
//     covers
//     // most common cases (multi-gpu/integrated+dedicated graphics). Handling more complicated setups
//     (multiple
//     // dedicated GPUs) is out of scope of this sample.
//     for (VkPhysicalDevice& device : gpus)
//     {
//         VkPhysicalDeviceProperties properties;
//         vkGetPhysicalDeviceProperties(device, &properties);
//         if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
//             return device;
//     }
//
//     // Use first GPU (Integrated) is a Discrete one is not available.
//     if (gpu_count > 0)
//         return gpus[0];
//     return VK_NULL_HANDLE;
// }
//
//     // Select Physical Device (GPU)
//     g_PhysicalDevice = SetupVulkan_SelectPhysicalDevice();
//
//     // Select graphics queue family
//     {
//         uint32_t count;
//         vkGetPhysicalDeviceQueueFamilyProperties(g_PhysicalDevice, &count, nullptr);
//         VkQueueFamilyProperties* queues = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties)
//         * count); vkGetPhysicalDeviceQueueFamilyProperties(g_PhysicalDevice, &count, queues); for (uint32_t
//         i = 0; i < count; i++)
//             if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
//             {
//                 g_QueueFamily = i;
//                 break;
//             }
//         free(queues);
//         IM_ASSERT(g_QueueFamily != (uint32_t)-1);
//     }
//
//     // Create Logical Device (with 1 queue)
//     {
//         ImVector<const char*> device_extensions;
//         device_extensions.push_back("VK_KHR_swapchain");
//
//         // Enumerate physical device extension
//         uint32_t properties_count;
//         ImVector<VkExtensionProperties> properties;
//         vkEnumerateDeviceExtensionProperties(g_PhysicalDevice, nullptr, &properties_count, nullptr);
//         properties.resize(properties_count);
//         vkEnumerateDeviceExtensionProperties(g_PhysicalDevice, nullptr, &properties_count,
//         properties.Data);
// #ifdef VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME
//         if (IsExtensionAvailable(properties, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME))
//             device_extensions.push_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
// #endif
//
//         const float queue_priority[] = { 1.0f };
//         VkDeviceQueueCreateInfo queue_info[1] = {};
//         queue_info[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
//         queue_info[0].queueFamilyIndex = g_QueueFamily;
//         queue_info[0].queueCount = 1;
//         queue_info[0].pQueuePriorities = queue_priority;
//         VkDeviceCreateInfo create_info = {};
//         create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
//         create_info.queueCreateInfoCount = sizeof(queue_info) / sizeof(queue_info[0]);
//         create_info.pQueueCreateInfos = queue_info;
//         create_info.enabledExtensionCount = (uint32_t)device_extensions.Size;
//         create_info.ppEnabledExtensionNames = device_extensions.Data;
//         err = vkCreateDevice(g_PhysicalDevice, &create_info, g_Allocator, &g_Device);
//         check_vk_result(err);
//         vkGetDeviceQueue(g_Device, g_QueueFamily, 0, &g_Queue);
//     }
//
//     // Create Descriptor Pool
//     {
//         VkDescriptorPoolSize pool_sizes[] =
//         {
//             { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
//             { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
//             { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
//             { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
//             { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
//             { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
//             { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
//             { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
//             { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
//             { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
//             { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
//         };
//         VkDescriptorPoolCreateInfo pool_info = {};
//         pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
//         pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
//         pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
//         pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
//         pool_info.pPoolSizes = pool_sizes;
//         err = vkCreateDescriptorPool(g_Device, &pool_info, g_Allocator, &g_DescriptorPool);
//         check_vk_result(err);
//     }
// }
//
//
// // All the ImGui_ImplVulkanH_XXX structures/functions are optional helpers used by the demo.
// // Your real engine/app may not use them.
// static void SetupVulkanWindow(ImGui_ImplVulkanH_Window* wd, VkSurfaceKHR surface, int width, int height)
// {
//     wd->Surface = surface;
//
//     // Check for WSI support
//     VkBool32 res;
//     vkGetPhysicalDeviceSurfaceSupportKHR(g_PhysicalDevice, g_QueueFamily, wd->Surface, &res);
//     if (res != VK_TRUE)
//     {
//         fprintf(stderr, "Error no WSI support on physical device 0\n");
//         exit(-1);
//     }
//
//     // Select Surface Format
//     const VkFormat requestSurfaceImageFormat[] = { VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM,
//     VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM }; const VkColorSpaceKHR requestSurfaceColorSpace =
//     VK_COLORSPACE_SRGB_NONLINEAR_KHR; wd->SurfaceFormat =
//     ImGui_ImplVulkanH_SelectSurfaceFormat(g_PhysicalDevice, wd->Surface, requestSurfaceImageFormat,
//     (size_t)IM_ARRAYSIZE(requestSurfaceImageFormat), requestSurfaceColorSpace);
//
//     // Select Present Mode
// #ifdef IMGUI_UNLIMITED_FRAME_RATE
//     VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR,
//     VK_PRESENT_MODE_FIFO_KHR };
// #else
//     VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_FIFO_KHR };
// #endif
//     wd->PresentMode = ImGui_ImplVulkanH_SelectPresentMode(g_PhysicalDevice, wd->Surface, &present_modes[0],
//     IM_ARRAYSIZE(present_modes));
//     //printf("[vulkan] Selected PresentMode = %d\n", wd->PresentMode);
//
//     // Create SwapChain, RenderPass, Framebuffer, etc.
//     IM_ASSERT(g_MinImageCount >= 2);
//     ImGui_ImplVulkanH_CreateOrResizeWindow(g_Instance, g_PhysicalDevice, g_Device, wd, g_QueueFamily,
//     g_Allocator, width, height, g_MinImageCount);
// }
//
// static void CleanupVulkan()
// {
//     vkDestroyDescriptorPool(g_Device, g_DescriptorPool, g_Allocator);
//
// #ifdef IMGUI_VULKAN_DEBUG_REPORT
//     // Remove the debug report callback
//     auto vkDestroyDebugReportCallbackEXT =
//     (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(g_Instance,
//     "vkDestroyDebugReportCallbackEXT"); vkDestroyDebugReportCallbackEXT(g_Instance, g_DebugReport,
//     g_Allocator);
// #endif // IMGUI_VULKAN_DEBUG_REPORT
//
//     vkDestroyDevice(g_Device, g_Allocator);
//     vkDestroyInstance(g_Instance, g_Allocator);
// }
//
// static void CleanupVulkanWindow()
// {
//     ImGui_ImplVulkanH_DestroyWindow(g_Instance, g_Device, &g_MainWindowData, g_Allocator);
// }
//
//
// static void FrameRender(ImGui_ImplVulkanH_Window* wd, ImDrawData* draw_data)
// {
//     VkResult err;
//
//     VkSemaphore image_acquired_semaphore  = wd->FrameSemaphores[wd->SemaphoreIndex].ImageAcquiredSemaphore;
//     VkSemaphore render_complete_semaphore =
//     wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore; err = vkAcquireNextImageKHR(g_Device,
//     wd->Swapchain, UINT64_MAX, image_acquired_semaphore, VK_NULL_HANDLE, &wd->FrameIndex); if (err ==
//     VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
//     {
//         g_SwapChainRebuild = true;
//         return;
//     }
//     check_vk_result(err);
//
//     ImGui_ImplVulkanH_Frame* fd = &wd->Frames[wd->FrameIndex];
//     {
//         err = vkWaitForFences(g_Device, 1, &fd->Fence, VK_TRUE, UINT64_MAX);    // wait indefinitely
//         instead of periodically checking check_vk_result(err);
//
//         err = vkResetFences(g_Device, 1, &fd->Fence);
//         check_vk_result(err);
//     }
//     {
//         err = vkResetCommandPool(g_Device, fd->CommandPool, 0);
//         check_vk_result(err);
//         VkCommandBufferBeginInfo info = {};
//         info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//         info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
//         err = vkBeginCommandBuffer(fd->CommandBuffer, &info);
//         check_vk_result(err);
//     }
//     {
//         VkRenderPassBeginInfo info = {};
//         info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
//         info.renderPass = wd->RenderPass;
//         info.framebuffer = fd->Framebuffer;
//         info.renderArea.extent.width = wd->Width;
//         info.renderArea.extent.height = wd->Height;
//         info.clearValueCount = 1;
//         info.pClearValues = &wd->ClearValue;
//         vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
//     }
//
//     // Record dear imgui primitives into command buffer
//     ImGui_ImplVulkan_RenderDrawData(draw_data, fd->CommandBuffer);
//
//     // Submit command buffer
//     vkCmdEndRenderPass(fd->CommandBuffer);
//     {
//         VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//         VkSubmitInfo info = {};
//         info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
//         info.waitSemaphoreCount = 1;
//         info.pWaitSemaphores = &image_acquired_semaphore;
//         info.pWaitDstStageMask = &wait_stage;
//         info.commandBufferCount = 1;
//         info.pCommandBuffers = &fd->CommandBuffer;
//         info.signalSemaphoreCount = 1;
//         info.pSignalSemaphores = &render_complete_semaphore;
//
//         err = vkEndCommandBuffer(fd->CommandBuffer);
//         check_vk_result(err);
//         err = vkQueueSubmit(g_Queue, 1, &info, fd->Fence);
//         check_vk_result(err);
//     }
// }
//
//
// static void FramePresent(ImGui_ImplVulkanH_Window* wd)
// {
//     if (g_SwapChainRebuild)
//         return;
//     VkSemaphore render_complete_semaphore =
//     wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore; VkPresentInfoKHR info = {}; info.sType
//     = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR; info.waitSemaphoreCount = 1; info.pWaitSemaphores =
//     &render_complete_semaphore; info.swapchainCount = 1; info.pSwapchains = &wd->Swapchain;
//     info.pImageIndices = &wd->FrameIndex;
//     VkResult err = vkQueuePresentKHR(g_Queue, &info);
//     if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
//     {
//         g_SwapChainRebuild = true;
//         return;
//     }
//     check_vk_result(err);
//     wd->SemaphoreIndex = (wd->SemaphoreIndex + 1) % wd->ImageCount; // Now we can use the next set of
//     semaphores
// }


namespace lue::view {

    VulkanApplication::VulkanApplication(std::vector<std::string> const& arguments):

        Application{arguments}

    {
    }


    VulkanApplication::~VulkanApplication()
    {
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


        // Initialize GLFW -----------------------------------------------------
        // TODO Optional, if a window is needed
        glfw::Library library{};
        glfw::Monitor monitor{};

        assert(glfwVulkanSupported());
        // / Optional, if a window is needed


        // Initialize Vulkan ---------------------------------------------------
        std::string const application_name{"lue_view"};

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

        // TODO Optional, if a window is needed
        vulkan::Names required_extension_names{glfw::Library::required_instance_extensions()};
        // / Optional, if a window is needed

#ifndef NDEBUG
        // required_extension_names.push_back("VK_EXT_debug_report");
        // required_extension_names.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        required_extension_names.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif


        // On macOS / Molten, creating an instance may result in VK_ERROR_INCOMPATIBLE_DRIVER. This
        // can be solved:
        // If using MacOS with the latest MoltenVK sdk, you may get VK_ERROR_INCOMPATIBLE_DRIVER
        // returned from vkCreateInstance. According to the Getting Start Notes. Be-
        // ginning with the 1.3.216 Vulkan SDK, the VK_KHR_PORTABILITY_subset
        // extension is mandatory.
        // - Add VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR to CreateInfo's flags
        // - Add VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME to required_extension_names
        // TODO Can we know beforehand if doing this is required and do it conditionally?


        /// vulkan::ExtensionProperties const available_extension_properties{
        ///     vulkan::Instance::extension_properties()};

        /// assert(std::all_of(required_extension_names.begin(), required_extension_names.end(),
        ///         [&available_extension_properties](auto const& required_extension_name)
        ///         {
        ///             return vulkan::Instance::extension_available(
        ///                 available_extension_properties, required_extension_name);
        ///         }
        ///     ));

        vulkan::Names required_layer_names{
#ifndef NDEBUG
            // Only available when the LunarG Vulkan SDK is installed
            "VK_LAYER_KHRONOS_validation"
#endif
        };

        /// vulkan::Instance::LayerProperties const
        /// available_layer_properties{vulkan::Instance::layer_properties()};

        /// assert(std::all_of(required_layer_names.begin(), required_layer_names.end(),
        ///         [&available_layer_properties](auto const& required_layer_name)
        ///         {
        ///             return vulkan::Instance::layer_available(
        ///                 available_layer_properties, required_layer_name);
        ///         }
        ///     ));

        // TODO Add debug print callback to validate create/destroy the instance itself
        vulkan::Instance instance{vulkan::Instance::CreateInfo{
            vulkan::ApplicationInfo{
                application_name,
                std::make_tuple(
                    BuildOptions::major_version, BuildOptions::minor_version, BuildOptions::patch_version),
                VK_API_VERSION_1_2},
            required_layer_names,
            required_extension_names}};

#ifndef NDEBUG
        vulkan::DebugReportCallback debug_callback{
            instance,
            vulkan::DebugReportCallback::CreateInfo{
                VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT |
                    VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT,
                debug_report}};
#endif

        // Look for and select the graphics card(s) that support(s) the features that we need

        vulkan::PhysicalDevices physical_devices{instance.physical_devices()};

        if (physical_devices.empty())
        {
            throw std::runtime_error("Failed to find GPUs with Vulkan support");
        }

        std::cout << "Physical devices:" << std::endl;

        for (auto const& device : physical_devices)
        {
            auto const properties{device.properties()};

            // std::cout << "    " << properties.TODO << std::endl;
            std::cout << "    TODO" << std::endl;
        }


        // Initialize window ---------------------------------------------------
        // Create window with Vulkan context
        glfw::Window::hint(GLFW_CLIENT_API, GLFW_NO_API);
        glfw::Window::hint(GLFW_RESIZABLE, GLFW_FALSE);  // TODO

        glfw::VideoMode const video_mode{monitor.video_mode()};
        glfw::Window::hint(GLFW_RED_BITS, video_mode.red_bits());
        glfw::Window::hint(GLFW_GREEN_BITS, video_mode.green_bits());
        glfw::Window::hint(GLFW_BLUE_BITS, video_mode.blue_bits());
        glfw::Window::hint(GLFW_REFRESH_RATE, video_mode.refresh_rate());

        glfw::Window glfw_window{
            "LUE view",
            static_cast<int>(std::ceil(0.9 * video_mode.width())),
            static_cast<int>(std::ceil(0.9 * video_mode.height()))};

        imgui::glfw::VulkanSurface glfw_surface{instance, glfw_window};


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


        // Main loop
        bool quit = glfw_window.should_close();

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

            quit = quit || glfw_window.should_close();


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
