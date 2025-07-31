#pragma once
#include <string>
#include <vector>


namespace lue {

    class CommandLine
    {

        public:

            CommandLine();

            [[nodiscard]] auto argc() const -> int;

            [[nodiscard]] auto argv() const -> char**;

        private:

            // For each argument a string
            std::vector<std::string> _argument_strings;

            // For each argument a pointer to the array of characters
            std::vector<char*> _argument_pointers;

            // Command line arguments, to be used by HPX runtime startup code
            int _argc;
            char** _argv;
    };

}  // namespace lue
