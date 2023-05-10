#pragma once
#include <GLFW/glfw3.h>


namespace lue::glfw {

    class VideoMode
    {

        public:

            VideoMode(GLFWvidmode const* video_mode);

            int width() const;

            int height() const;

            int red_bits() const;

            int green_bits() const;

            int blue_bits() const;

            int refresh_rate() const;

        private:

            GLFWvidmode const* _video_mode;
    };

}  // namespace lue::glfw
