#pragma once
#include "lue/imgui/glfw/binding.hpp"
#include "lue/glfw/window.hpp"
#include <imgui.h>


namespace lue::imgui::glfw {

    class OpenGLBinding: public Binding
    {

        public:

            OpenGLBinding(std::string const& glsl_version, lue::glfw::Window& window);

            ~OpenGLBinding() override;
    };

}  // namespace lue::imgui::glfw
