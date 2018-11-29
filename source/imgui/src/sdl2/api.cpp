#include "lue/gui/sdl2/api.hpp"
#include <stdexcept>


namespace sdl2 {

API::API()
{
    // Setup SDL
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        throw std::runtime_error(SDL_GetError());
    }
}


API::~API()
{
    SDL_Quit();
}

}  // namespace sdl2
