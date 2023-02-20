#include "lue/utility/application.hpp"


namespace lue {
    namespace utility {

        Application::Application(std::string const& usage, std::vector<std::string> const& arguments)

            :
            Command(usage, arguments)

        {
        }


        Application::Application(
            std::string const& usage,
            std::vector<std::string> const& arguments,
            SubcommandCreators const& subcommand_creators)

            :
            Command(usage, arguments, subcommand_creators)

        {
        }

    }  // namespace utility
}  // namespace lue
