#pragma once
#include <SDL.h>
#include <string>


namespace sdl2 {

class Window
{

public:

                   Window              (std::string const& title,
                                        int const x=SDL_WINDOWPOS_CENTERED,
                                        int const y=SDL_WINDOWPOS_CENTERED,
                                        int const w=600,
                                        int const h=400,
                                        Uint32 const flags=
                                    SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);

                   ~Window             ();

                   operator SDL_Window*();

    SDL_GLContext  gl_context          ();

private:

    SDL_Window*    _window;

    SDL_GLContext  _gl_context;

};

}  // namespace sdl2
