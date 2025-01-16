#include "lue/imgui/glfw/vulkan_binding.hpp"
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>


namespace lue::imgui::glfw {

    VulkanBinding::VulkanBinding([[maybe_unused]] lue::glfw::Window& window):

        Binding{}

    {
        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForVulkan(window, true);
    }


    VulkanBinding::~VulkanBinding()
    {
        ImGui_ImplVulkan_Shutdown();
    }

}  // namespace lue::imgui::glfw
