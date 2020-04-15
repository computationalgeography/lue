#include "lue/gui/tab_bar.hpp"
#include <imgui.h>


namespace lue {
namespace gui {

TabBar::TabBar(
    std::string const& name):

    Selectable{ImGui::BeginTabBar(name.c_str())}

{
}


TabBar::~TabBar()
{
    if(was_selected()) {
        ImGui::EndTabBar();
    }
}

}  // namespace gui
}  // namespace lue
