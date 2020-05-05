#pragma once
#include <SDL.h>
#include <string>


namespace sdl2 {

class Window
{

public:

    explicit       Window              (std::string const& title,
                                        int x=SDL_WINDOWPOS_CENTERED,
                                        int y=SDL_WINDOWPOS_CENTERED,
                                        int w=640,
                                        int h=480,
                                        Uint32 flags=
                                            SDL_WINDOW_OPENGL |
                                            SDL_WINDOW_RESIZABLE);

                   ~Window             ();

                   operator SDL_Window*();

    SDL_GLContext  gl_context          ();

private:

    SDL_Window*    _window;

    SDL_GLContext  _gl_context;

};

}  // namespace sdl2
