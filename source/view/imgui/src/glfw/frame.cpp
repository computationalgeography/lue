#include "lue/imgui/glfw/frame.hpp"
#include <imgui.h>


namespace lue::imgui::glfw {

    Frame::Frame(lue::glfw::Window& window):

        _window{window}

    {
    }


    Frame::~Frame()
    {
    }

}  // namespace lue::imgui::glfw
