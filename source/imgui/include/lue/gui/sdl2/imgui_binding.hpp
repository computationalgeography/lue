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

                   ~ImGuiBinding       ();

    ImGuiIO&       io                  ();

private:

};

}  // namespace sdl2
