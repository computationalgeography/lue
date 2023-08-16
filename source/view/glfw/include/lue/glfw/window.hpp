#pragma once
#include "lue/glfw/monitor.hpp"
#include <string>


namespace lue::glfw {

    class Window
    {

        public:

            static void hint(int hint, int value);

            Window(std::string const& title, int width, int height);

            Window(std::string const& title, int width, int height, Monitor& monitor);

            // Window(std::string const& title, Monitor& monitor);

            Window(Window const&) = delete;

            Window(Window&&) = delete;

            ~Window();

            Window& operator=(Window const&) = delete;

            Window& operator=(Window&&) = delete;

            operator GLFWwindow*();

            void make_context_current();

            std::tuple<int, int> framebuffer_size() const;

            bool should_close() const;

            void set_user_pointer(void* pointer);

            GLFWframebuffersizefun set_framebuffer_size_callback(GLFWframebuffersizefun callback);

        private:

            GLFWwindow* _window;
    };

}  // namespace lue::glfw
