#pragma once
#include <GLFW/glfw3.h>
#include <string>


namespace lue::glfw {

    class Window
    {

        public:

            explicit Window(
                std::string const& title,
                int w = 640,
                int h = 480,
                GLFWmonitor* monitor = nullptr,
                GLFWwindow* share = nullptr);

            Window(Window const&) = delete;

            Window(Window&&) = delete;

            ~Window();

            Window& operator=(Window const&) = delete;

            Window& operator=(Window&&) = delete;

            operator GLFWwindow*();

        private:

            GLFWwindow* _window;
    };

}  // namespace lue::glfw
