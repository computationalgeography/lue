#include "lue/imgui/glfw/opengl_binding.hpp"
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


namespace lue::imgui::glfw {

    OpenGLBinding::OpenGLBinding(std::string const& glsl_version, lue::glfw::Window& window):

        Binding{}

    {
        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version.c_str());
    }


    OpenGLBinding::~OpenGLBinding()
    {
        ImGui_ImplOpenGL3_Shutdown();
    }

}  // namespace lue::imgui::glfw
