#pragma once
#include "lue/glfw/window.hpp"


namespace lue::imgui::glfw {

    class Frame
    {

        public:

            explicit Frame(lue::glfw::Window& window);

            Frame(Frame const&) = delete;

            Frame(Frame&&) = delete;

            ~Frame();

            Frame& operator=(Frame const&) = delete;

            Frame& operator=(Frame&&) = delete;

        private:

            lue::glfw::Window& _window;
    };

}  // namespace lue::imgui::glfw
