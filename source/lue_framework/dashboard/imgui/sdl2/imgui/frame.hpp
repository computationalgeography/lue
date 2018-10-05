#pragma once
#include "sdl2/window.hpp"


namespace sdl2 {
namespace imgui {

class Frame
{

public:

                   Frame               (sdl2::Window& window);

                   ~Frame              ();

private:

    sdl2::Window&  _window;

};

}  // namespace imgui
}  // namespace sdl2
