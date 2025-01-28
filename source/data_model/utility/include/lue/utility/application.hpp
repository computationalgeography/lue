#pragma once
#include "lue/utility/command.hpp"


namespace lue::utility {

    class Application: public Command
    {

        public:

            Application(Application const&) = delete;

            Application(Application&&) = delete;

            ~Application() override = default;

            auto operator=(Application const&) -> Application& = delete;

            auto operator=(Application&&) -> Application& = delete;

        protected:

            Application(cxxopts::Options&& options, int argc, char const* const* argv);

            Application(
                cxxopts::Options&& options,
                int argc,
                char const* const* argv,
                SubcommandCreators&& subcommand_creators);

        private:
    };

}  // namespace lue::utility
