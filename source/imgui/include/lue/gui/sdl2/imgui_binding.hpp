#pragma once
#include "lue/gui/sdl2/api.hpp"
#include "lue/gui/sdl2/window.hpp"
#include "imgui.h"


namespace sdl2 {

class ImGuiBinding
{

public:

                   ImGuiBinding        (API const& api,
                                        Window& window);

                   ImGuiBinding        (ImGuiBinding const&)=default;

                   ImGuiBinding        (ImGuiBinding&&)=default;

                   ~ImGuiBinding       ();

    ImGuiBinding&  operator=           (ImGuiBinding const&)=default;

    ImGuiBinding&  operator=           (ImGuiBinding&&)=default;

    static ImGuiIO& io()
    {
        return ImGui::GetIO();
    }

private:

};

}  // namespace sdl2
