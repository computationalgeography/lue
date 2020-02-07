#pragma once
#include <SDL.h>
#include <string>


namespace sdl2 {

class API
{

public:

                   API                 ();

                   ~API                ();

    std::string const& glsl_version    () const;

private:

    std::string    _glsl_version;

};

}  // namespace sdl2
