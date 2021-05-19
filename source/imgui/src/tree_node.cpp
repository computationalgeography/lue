#include "lue/gui/tree_node.hpp"


namespace lue::gui {

    TreeNode::TreeNode(
        std::string const& name,
        ImGuiTreeNodeFlags const flags):

        Selectable{ImGui::TreeNodeEx(name.c_str(), flags)}

    {
    }


    TreeNode::~TreeNode()
    {
        if(was_selected())
        {
            ImGui::TreePop();
        }
    }

}  // namespace lue::gui
