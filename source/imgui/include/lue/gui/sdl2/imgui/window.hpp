#pragma once
#include <string>


namespace sdl2 {
namespace imgui {

class Window
{

public:

                   Window               (std::string const& title,
                                         bool* open=nullptr);

                   ~Window              ();

private:

};

}  // namespace imgui
}  // namespace sdl2
