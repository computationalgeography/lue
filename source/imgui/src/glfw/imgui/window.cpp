#include "lue/gui/glfw/imgui/window.hpp"
#include "imgui.h"


namespace glfw::imgui {

    Window::Window(
        std::string const& title,
        bool* open)
    {
        ImGui::Begin(title.c_str(), open);
    }


    Window::~Window()
    {
        ImGui::End();
    }

}  // namespace glfw::namespace imgui
