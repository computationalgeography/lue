#pragma once
#include "lue/imgui/selectable.hpp"
#include <string>


namespace lue::imgui {

    class Menu: public Selectable
    {

        public:

            explicit Menu(std::string const& label, bool enabled = true);

            ~Menu() override;
    };

}  // namespace lue::imgui
