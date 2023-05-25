#include "lue/imgui/glfw/opengl_frame.hpp"
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


namespace lue::imgui::glfw {

    OpenGLFrame::OpenGLFrame(lue::glfw::Window& window):

        Frame{window}

    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }


    OpenGLFrame::~OpenGLFrame()
    {
        ImGui::Render();

        auto& io = ImGui::GetIO();
        glViewport(0, 0, static_cast<int>(io.DisplaySize.x), static_cast<int>(io.DisplaySize.y));
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window());
    }

}  // namespace lue::imgui::glfw
