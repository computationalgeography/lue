#include "lue/imgui/window.hpp"


namespace lue::imgui {

    Window::Window(std::string const& title, bool* open, ImGuiWindowFlags flags):

        Selectable{ImGui::Begin(title.c_str(), open, flags)}

    {
    }


    Window::~Window()
    {
        // In case of Begin, whether or not true was returned is not
        // relevant. Always call End.
        ImGui::End();
    }

}  // namespace lue::imgui
