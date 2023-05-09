#include "lue/imgui/menu.hpp"
#include <imgui.h>


namespace lue::imgui {

    Menu::Menu(std::string const& label, bool const enabled):

        Selectable{ImGui::BeginMenu(label.c_str(), enabled)}

    {
    }


    Menu::~Menu()
    {
        if (was_selected())
        {
            ImGui::EndMenu();
        }
    }

}  // namespace lue::imgui
