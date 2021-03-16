#pragma once
#include "lue/gui/glfw/window.hpp"


namespace glfw::imgui {

    class Frame
    {

        public:

            explicit Frame(glfw::Window& window);

            Frame(Frame const&)=delete;

            Frame(Frame&&)=delete;

            ~Frame();

            Frame& operator=(Frame const&)=delete;

            Frame& operator=(Frame&&)=delete;

        private:

            glfw::Window&  _window;

    };

}  // namespace glfw::imgui
