#pragma once
#include "lue/gui/selectable.hpp"
#include <string>


namespace lue::gui {

    class TabItem: public Selectable
    {

        public:

            explicit TabItem(std::string const& name);

            TabItem(TabItem const&) = default;

            TabItem(TabItem&&) = default;

            ~TabItem() override;

            TabItem& operator=(TabItem const&) = default;

            TabItem& operator=(TabItem&&) = default;
    };

}  // namespace lue::gui
