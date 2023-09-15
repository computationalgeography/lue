#include "lue/imgui/glfw/vulkan_frame.hpp"
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>


namespace lue::imgui::glfw {

    VulkanFrame::VulkanFrame(lue::glfw::Window& window):

        Frame{window}

    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }


    VulkanFrame::~VulkanFrame()
    {
    }

}  // namespace lue::imgui::glfw
