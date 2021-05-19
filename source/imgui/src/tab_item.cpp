#include "lue/gui/tab_item.hpp"
#include <imgui.h>


namespace lue::gui {

    TabItem::TabItem(
        std::string const& name):

        Selectable{ImGui::BeginTabItem(name.c_str())}

    {
    }


    TabItem::~TabItem()
    {
        if(was_selected())
        {
            ImGui::EndTabItem();
        }
    }

}  // namespace lue::gui
