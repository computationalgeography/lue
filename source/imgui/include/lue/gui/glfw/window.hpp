#pragma once
#include <GLFW/glfw3.h>
#include <string>


namespace glfw {

    class Window
    {

        public:

            explicit Window(
                std::string const& title,
                int w=640,
                int h=480,
                GLFWmonitor* monitor=nullptr,
                GLFWwindow* share=nullptr);

            Window(Window const&)=default;

            Window(Window&&)=default;

            ~Window();

            Window& operator=(Window const&)=default;

            Window& operator=(Window&&)=default;

            operator GLFWwindow*();

        private:

            GLFWwindow* _window;

    };

}  // namespace glfw
