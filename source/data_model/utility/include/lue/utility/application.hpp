#pragma once
#include "lue/utility/command.hpp"
#include "lue/utility/export.hpp"


namespace lue::utility {

    class LUE_UTILITY_EXPORT Application: public Command
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
