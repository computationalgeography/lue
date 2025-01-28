#include "lue/utility/application.hpp"


namespace lue::utility {

    Application::Application(cxxopts::Options&& options, int argc, char const* const* argv):

        Command(std::move(options), argc, argv)

    {
    }


    Application::Application(
        cxxopts::Options&& options,
        int argc,
        char const* const* argv,
        SubcommandCreators&& subcommand_creators):

        Command(std::move(options), argc, argv, std::move(subcommand_creators))

    {
    }

}  // namespace lue::utility
