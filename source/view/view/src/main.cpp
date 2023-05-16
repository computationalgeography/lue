#include "lue/view/opengl_application.hpp"
// #include "lue/view/vulkan_application.hpp"
#include <iostream>


int main(int argc, char* argv[])
{
    int result{EXIT_FAILURE};

    try
    {
        // result = lue::view::VulkanApplication({argv + 1, argv + argc}).run();
        result = lue::view::OpenGLApplication({argv + 1, argv + argc}).run();
    }
    catch (std::exception const& exception)
    {
        std::cerr << exception.what() << std::endl;
    }

    return result;
}
