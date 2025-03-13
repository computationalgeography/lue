#include "lue/glfw/library.hpp"
#include <GLFW/glfw3.h>
#include <format>
#include <stdexcept>


namespace lue::glfw {

    static void glfw_error_callback(int const error, char const* description)
    {
        throw std::runtime_error(std::format("GLFW error {}: {})", error, description));
    }


    /*!
        @brief      Return the version of the runtime library
    */
    std::tuple<int, int, int> Library::version()
    {
        int major, minor, revision;

        ::glfwGetVersion(&major, &minor, &revision);

        return {major, minor, revision};
    }


    /*!
        @brief      Return the version of the compile-time library
    */
    std::string Library::version_string()
    {
        static std::string const string{::glfwGetVersionString()};

        return string;
    }


    void Library::hint(int hint, int value)
    {
        ::glfwInitHint(hint, value);
    }


    void Library::poll_events()
    {
        ::glfwPollEvents();
    }


    /*!
        @brief      Return names of Vulkan extensions GLFW needs to be able to interact with
                    the window system
    */
    std::vector<char const*> Library::required_instance_extensions()
    {
        std::uint32_t nr_extensions;

        auto names = ::glfwGetRequiredInstanceExtensions(&nr_extensions);

        return std::vector<char const*>(names, names + nr_extensions);
    }


    Library::Library()
    {
        ::glfwSetErrorCallback(glfw_error_callback);
        ::glfwInit();
    }


    Library::~Library()
    {
        ::glfwTerminate();
    }

}  // namespace lue::glfw
