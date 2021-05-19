#pragma once
#include "lue/gui/selectable.hpp"
#include <imgui.h>
#include <string>


namespace lue::gui {

    class Window:
        public Selectable
    {

        public:

            explicit Window(
                std::string const& title,
                bool* open=nullptr,
                ImGuiWindowFlags flags=0);

            Window(Window const&)=default;

            Window(Window&&)=default;

            ~Window();

            Window& operator=(Window const&)=default;

            Window& operator=(Window&&)=default;

    };

}  // namespace lue::gui
