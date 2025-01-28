#pragma once
#include "lue/utility/application.hpp"


namespace lue::utility {

    class Validate: public Application

    {

        public:

            Validate(int argc, char const* const* argv);

            auto run_implementation() -> int final;

        private:
    };

}  // namespace lue::utility
