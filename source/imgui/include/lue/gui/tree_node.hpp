#pragma once
#include "lue/gui/selectable.hpp"
#include <imgui.h>
#include <string>


namespace lue::gui {

    class TreeNode: public Selectable
    {

        public:

            explicit TreeNode(std::string const& name, ImGuiTreeNodeFlags flags = 0);

            TreeNode(TreeNode const&) = default;

            TreeNode(TreeNode&&) = default;

            ~TreeNode() override;

            TreeNode& operator=(TreeNode const&) = default;

            TreeNode& operator=(TreeNode&&) = default;
    };

}  // namespace lue::gui
