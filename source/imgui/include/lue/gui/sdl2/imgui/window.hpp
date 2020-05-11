#pragma once
#include <string>


namespace sdl2 {
namespace imgui {

class Window
{

public:

    explicit       Window               (std::string const& title,
                                         bool* open=nullptr);

                   Window               (Window const&)=default;

                   Window               (Window&&)=default;

                   ~Window              ();

    Window&        operator=            (Window const&)=default;

    Window&        operator=            (Window&&)=default;

private:

};

}  // namespace imgui
}  // namespace sdl2
