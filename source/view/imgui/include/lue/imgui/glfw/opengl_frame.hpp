#pragma once
#include "lue/imgui/glfw/frame.hpp"


namespace lue::imgui::glfw {

    class OpenGLFrame: public Frame
    {

        public:

            OpenGLFrame(lue::glfw::Window& window);

            ~OpenGLFrame() override;
    };

}  // namespace lue::imgui::glfw
