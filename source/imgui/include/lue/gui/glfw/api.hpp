#pragma once
#include <string>


namespace glfw {

    class API
    {

        public:

            API();

            API(API const&) = default;

            API(API&&) = default;

            ~API();

            API& operator=(API const&) = default;

            API& operator=(API&&) = default;

            std::string const& glsl_version() const;

        private:

            std::string _glsl_version;
    };

}  // namespace glfw
