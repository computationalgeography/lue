#include "lue/gui/sdl2/window.hpp"
#include <SDL_opengl.h>
#include <cassert>
#include <stdexcept>


namespace sdl2 {

Window::Window(
    std::string const& title,
    int const x,
    int const y,
    int const w,
    int const h,
    Uint32 const flags):

    _window{nullptr},
    _gl_context{nullptr}

{
    // Create window with graphics context
    // https://wiki.libsdl.org/SDL_GL_SetAttribute
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    _window = SDL_CreateWindow(title.c_str(), x, y, w, h, flags);
    assert(_window);

    _gl_context = SDL_GL_CreateContext(_window);
    assert(_gl_context);

    SDL_GL_MakeCurrent(_window, _gl_context);
    SDL_GL_SetSwapInterval(1);  // Enable vsync
}


Window::~Window()
{
    SDL_GL_DeleteContext(_gl_context);
    SDL_DestroyWindow(_window);
}


Window::operator SDL_Window*() {
    return _window;
}


SDL_GLContext Window::gl_context()
{
    return _gl_context;
}

}  // namespace sdl2
