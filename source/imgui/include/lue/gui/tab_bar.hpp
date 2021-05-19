#pragma once
#include "lue/gui/selectable.hpp"
#include <string>


namespace lue::gui {

    class TabBar:
        public Selectable
    {

        public:

            explicit TabBar(std::string const& name);

            TabBar(TabBar const&)=default;

            TabBar(TabBar&&)=default;

            ~TabBar() override;

            TabBar& operator=(TabBar const&)=default;

            TabBar& operator=(TabBar&&)=default;

    };

}  // namespace lue::gui
