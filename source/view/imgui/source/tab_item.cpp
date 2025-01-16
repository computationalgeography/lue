#include "lue/imgui/tab_item.hpp"
#include <imgui.h>


namespace lue::imgui {

    TabItem::TabItem(std::string const& name):

        Selectable{ImGui::BeginTabItem(name.c_str())}

    {
    }


    TabItem::~TabItem()
    {
        if (was_selected())
        {
            ImGui::EndTabItem();
        }
    }

}  // namespace lue::imgui
