#pragma once
#include <string>
#include <tuple>


namespace lue::glfw {

    class Library
    {

        public:

            static std::tuple<int, int, int> version();

            static std::string version_string();

            static void hint(int hint, int value);

            Library();

            ~Library();

        private:
    };

}  // namespace lue::glfw
