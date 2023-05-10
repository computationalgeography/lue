#include "lue/glfw/window.hpp"
#include <cassert>
#include <stdexcept>


namespace lue::glfw {

    void Window::hint(int hint, int value)
    {
        glfwWindowHint(hint, value);
    }


    Window::Window(std::string const& title, int const width, int const height):

        _window{glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr)}

    {
        // Windowed mode window
        assert(_window);
    }


    Window::Window(std::string const& title, int const width, int const height, Monitor& monitor):

        _window{glfwCreateWindow(width, height, title.c_str(), monitor, nullptr)}

    {
        // Windowed full screen window
        assert(_window);
    }


    // TODO Use video mode
    // Window::Window(std::string const& title, Monitor& monitor):

    //     _window{nullptr}

    // {
    //     auto const [x, y, width, height] = monitor.work_area();

    //     _window = glfwCreateWindow(width, height, title.c_str(), monitor, nullptr);

    //     // Fullscreen mode window
    //     assert(_window);
    // }


    Window::~Window()
    {
        glfwDestroyWindow(_window);
    }


    Window::operator GLFWwindow*()
    {
        return _window;
    }


    void Window::make_context_current()
    {
        glfwMakeContextCurrent(_window);
    }


    std::tuple<int, int> Window::framebuffer_size() const
    {
        int width, height;

        glfwGetFramebufferSize(_window, &width, &height);

        return {width, height};
    }

}  // namespace lue::glfw
