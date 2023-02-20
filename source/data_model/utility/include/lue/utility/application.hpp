#pragma once
#include "lue/utility/command.hpp"


namespace lue {
    namespace utility {

        class Application: public Command
        {

            public:

                Application(Application const&) = delete;

                Application(Application&&) = delete;

                ~Application() override = default;

                Application& operator=(Application const&) = delete;

                Application& operator=(Application&&) = delete;

            protected:

                Application(std::string const& usage, std::vector<std::string> const& arguments);

                Application(
                    std::string const& usage,
                    std::vector<std::string> const& arguments,
                    SubcommandCreators const& subcommand_creators);

            private:
        };

    }  // namespace utility
}  // namespace lue
