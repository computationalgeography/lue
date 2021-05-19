#include "lue/gui/glfw/api.hpp"
#include <GLFW/glfw3.h>
#include <fmt/format.h>
#include <stdexcept>


namespace glfw {

    static void glfw_error_callback(int error, const char* description)
    {
        // fprintf(stderr, "Glfw Error %d: %s\n", error, description);
        throw std::runtime_error(
            fmt::format("Glfw error {}: {}", error, description));
    }


    API::API():

        _glsl_version{}

    {
        glfwSetErrorCallback(glfw_error_callback);
        glfwInit();

        // Decide GL+GLSL versions
#ifdef __APPLE__
        // GL 3.2 + GLSL 150
        _glsl_version = "#version 150";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
        // GL 3.0 + GLSL 130
        _glsl_version = "#version 130";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif
    }


    API::~API()
    {
        glfwTerminate();
    }


    std::string const& API::glsl_version() const
    {
        return _glsl_version;
    }

}  // namespace glfw
