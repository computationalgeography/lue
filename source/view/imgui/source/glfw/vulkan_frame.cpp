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
        ImGui::Render();

        // TODO
        // auto& io = ImGui::GetIO();
        // glViewport(0, 0, static_cast<int>(io.DisplaySize.x), static_cast<int>(io.DisplaySize.y));
        // glClear(GL_COLOR_BUFFER_BIT);

        // ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // glfwSwapBuffers(window());
    }

}  // namespace lue::imgui::glfw
