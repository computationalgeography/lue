#include "lue/glfw/video_mode.hpp"
#include <cassert>


namespace lue::glfw {

    VideoMode::VideoMode(GLFWvidmode const* video_mode):

        _video_mode{video_mode}

    {
        assert(_video_mode);
    }


    int VideoMode::width() const
    {
        return _video_mode->width;
    }


    int VideoMode::height() const
    {
        return _video_mode->height;
    }


    int VideoMode::red_bits() const
    {
        return _video_mode->redBits;
    }


    int VideoMode::green_bits() const
    {
        return _video_mode->greenBits;
    }


    int VideoMode::blue_bits() const
    {
        return _video_mode->blueBits;
    }


    int VideoMode::refresh_rate() const
    {
        return _video_mode->refreshRate;
    }

}  // namespace lue::glfw
