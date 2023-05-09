#pragma once
#include "lue/imgui/selectable.hpp"
#include <imgui.h>
#include <string>


namespace lue::imgui {

    class Window: public Selectable
    {

        public:

            explicit Window(std::string const& title, bool* open = nullptr, ImGuiWindowFlags flags = 0);

            ~Window();
    };

}  // namespace lue::imgui
