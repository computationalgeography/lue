#include "sdl2/imgui/window.hpp"
#include "imgui.h"


namespace sdl2 {
namespace imgui {

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

}  // namespace imgui
}  // namespace sdl2
