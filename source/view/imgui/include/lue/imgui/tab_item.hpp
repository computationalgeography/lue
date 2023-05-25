#pragma once
#include "lue/imgui/selectable.hpp"
#include <string>


namespace lue::imgui {

    class TabItem: public Selectable
    {

        public:

            explicit TabItem(std::string const& name);

            ~TabItem() override;
    };

}  // namespace lue::imgui
