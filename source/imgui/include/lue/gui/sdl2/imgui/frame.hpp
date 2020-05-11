#pragma once
#include "lue/gui/sdl2/window.hpp"


namespace sdl2 {
namespace imgui {

class Frame
{

public:

    explicit       Frame               (sdl2::Window& window);

                   Frame               (Frame const&)=delete;

                   Frame               (Frame&&)=delete;

                   ~Frame              ();

    Frame&         operator=           (Frame const&)=delete;

    Frame&         operator=           (Frame&&)=delete;

private:

    sdl2::Window&  _window;

};

}  // namespace imgui
}  // namespace sdl2
