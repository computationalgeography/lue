#include "lue/gui/glfw/window.hpp"
#include <cassert>
#include <stdexcept>


namespace glfw {

    Window::Window(std::string const& title, int w, int h, GLFWmonitor* monitor, GLFWwindow* share):

        _window{nullptr}

    {
        _window = glfwCreateWindow(w, h, title.c_str(), monitor, share);
        assert(_window);

        glfwMakeContextCurrent(_window);
        glfwSwapInterval(1);  // Enable vsync
    }


    Window::~Window()
    {
        glfwDestroyWindow(_window);
    }


    Window::operator GLFWwindow*()
    {
        return _window;
    }

}  // namespace glfw
