#include "lue/gui/glfw/imgui/frame.hpp"
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <GLFW/glfw3.h>


namespace glfw::imgui {

    Frame::Frame(
        glfw::Window& window):

        _window{window}

    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }


    Frame::~Frame()
    {
        // Rendering
        ImGui::Render();

        auto& io = ImGui::GetIO();
        glViewport(
            0, 0,
            static_cast<int>(io.DisplaySize.x), static_cast<int>(io.DisplaySize.y));
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(_window);
    }

}  // namespace glfw::imgui
