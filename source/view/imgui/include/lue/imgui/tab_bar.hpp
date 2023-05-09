#pragma once
#include "lue/imgui/selectable.hpp"
#include <string>


namespace lue::imgui {

    class TabBar: public Selectable
    {

        public:

            explicit TabBar(std::string const& name);

            ~TabBar() override;
    };

}  // namespace lue::imgui
