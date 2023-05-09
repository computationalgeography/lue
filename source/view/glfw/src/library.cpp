#include "lue/glfw/library.hpp"
#include <fmt/format.h>
#include <GLFW/glfw3.h>
#include <stdexcept>


namespace lue::glfw {

    static void glfw_error_callback(int const error, char const* description)
    {
        throw std::runtime_error(fmt::format("GLFW error {}: {}", error, description));
    }


    Library::Library()
    {
        glfwSetErrorCallback(glfw_error_callback);
        glfwInit();
    }


    Library::~Library()
    {
        glfwTerminate();
    }

}  // namespace lue::glfw
