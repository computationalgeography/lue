#pragma once
#include "lue/view/application.hpp"


namespace lue::view {

    class OpenGLApplication: public Application
    {

        public:

            OpenGLApplication(std::vector<std::string> const& arguments);

            ~OpenGLApplication();

        protected:

            int run_implementation() final;

        private:
    };

}  // namespace lue::view
