// #include "lue/view/opengl_application.hpp"
#include "lue/view/vulkan_application.hpp"
#include <iostream>


auto main(int argc, char* argv[]) -> int
{
    int result{EXIT_FAILURE};

    try
    {
        result = lue::view::VulkanApplication{argc, argv}.run();
        // result = lue::view::OpenGLApplication{argc, argv}.run();
    }
    catch (std::exception const& exception)
    {
        std::cerr << exception.what() << std::endl;
    }

    return result;
}
