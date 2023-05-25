#pragma once
#include "lue/glfw/window.hpp"


namespace lue::imgui::glfw {

    class Frame
    {

        public:

            Frame(Frame const&) = delete;

            Frame(Frame&&) = delete;

            virtual ~Frame();

            Frame& operator=(Frame const&) = delete;

            Frame& operator=(Frame&&) = delete;

        protected:

            Frame(lue::glfw::Window& window);

            inline lue::glfw::Window& window()
            {
                return _window;
            };

        private:

            lue::glfw::Window& _window;
    };

}  // namespace lue::imgui::glfw
