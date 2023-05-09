#pragma once
#include "lue/imgui/selectable.hpp"
#include <imgui.h>
#include <string>


namespace lue::imgui {

    class TreeNode: public Selectable
    {

        public:

            explicit TreeNode(std::string const& name, ImGuiTreeNodeFlags flags = 0);

            ~TreeNode() override;
    };

}  // namespace lue::imgui
