#pragma once
#include "lue/view/application.hpp"


namespace lue::view {

    class OpenGLApplication: public Application
    {

        public:

            OpenGLApplication(int argc, char const* const* argv);

            ~OpenGLApplication();

        protected:

            auto run_implementation() -> int final;
    };

}  // namespace lue::view
