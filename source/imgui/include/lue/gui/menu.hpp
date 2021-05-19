#pragma once
#include "lue/gui/selectable.hpp"
#include <string>


namespace lue::gui {

    class Menu:
        public Selectable
    {

        public:

            explicit Menu(
                std::string const& label,
                bool enabled=true);

            Menu(Menu const&)=default;

            Menu(Menu&&)=default;

            ~Menu() override;

            Menu& operator=(Menu const&)=default;

            Menu& operator=(Menu&&)=default;

    };

}  // namespace lue::gui
