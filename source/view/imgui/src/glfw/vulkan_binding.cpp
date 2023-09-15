#include "lue/imgui/glfw/vulkan_binding.hpp"
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>


namespace lue::imgui::glfw {

    VulkanBinding::VulkanBinding(
        lue::glfw::Window& window,
        vulkan::Instance& instance,
        vulkan::PhysicalDevice& physical_device,
        vulkan::Device& device,
        vulkan::Queue& queue,
        vulkan::DescriptorPool& pool,
        // VkPipelineCache pipeline_cache,
        int const nr_images,
        vulkan::RenderPass& render_pass):

        Binding{}

    {
        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForVulkan(window, true);

        ImGui_ImplVulkan_InitInfo init_info{};
        init_info.Instance = instance;
        init_info.PhysicalDevice = physical_device;
        init_info.Device = device;
        init_info.QueueFamily = 55555;  // Random number. Not used by ImGUI?!
        init_info.Queue = queue;

        // init_info.PipelineCache = pipeline_cache;

        init_info.DescriptorPool = pool;

        // init_info.Subpass = 0;

        init_info.MinImageCount = nr_images;  // TODO Does this make sense?
        init_info.ImageCount = nr_images;

        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

        init_info.Allocator = nullptr;

        init_info.CheckVkResultFn = vulkan::assert_result_is_ok;

        ImGui_ImplVulkan_Init(&init_info, render_pass);

        // TODO Only now the fonts?
    }


    VulkanBinding::~VulkanBinding()
    {
        ImGui_ImplVulkan_Shutdown();
    }

}  // namespace lue::imgui::glfw
