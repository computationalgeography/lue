#pragma once
#include <string>
#include <tuple>
#include <vector>


namespace lue::glfw {

    class Library
    {

        public:

            static std::tuple<int, int, int> version();

            static std::string version_string();

            static void hint(int hint, int value);

            static void poll_events();

            static std::vector<char const*> required_instance_extensions();

            Library();

            ~Library();

        private:
    };

}  // namespace lue::glfw
