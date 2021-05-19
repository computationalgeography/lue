#include "lue/gui/tab_bar.hpp"
#include <imgui.h>


namespace lue::gui {

    TabBar::TabBar(
        std::string const& name):

        Selectable{ImGui::BeginTabBar(name.c_str())}

    {
    }


    TabBar::~TabBar()
    {
        if(was_selected())
        {
            ImGui::EndTabBar();
        }
    }

}  // namespace lue::gui
