#include "lue/gui/tree_node.hpp"


namespace lue {
namespace gui {

TreeNode::TreeNode(
    std::string const& name,
    ImGuiTreeNodeFlags const flags):

    Selectable{ImGui::TreeNodeEx(name.c_str(), flags)}

{
}


TreeNode::~TreeNode()
{
    if(was_selected()) {
        ImGui::TreePop();
    }
}

}  // namespace gui
}  // namespace lue
