#pragma once
#include <cxxopts.hpp>
#include <functional>
#include <map>
#include <memory>
#include <ostream>


namespace lue::utility {

    /*!
        @brief      Class representing a commandline command

        A commandline command encapsulates the actions to be taken by a commandline application. Some
        commandline applications (like Git) support subcommands. This is also supported. An application is a
        command, possibly using subcommands to implement parts of the functionality. Subcommands can also use
        subcommands, etc.
    */
    class Command
    {

        public:

            /*!
                @brief      Pointer to a command
            */
            using CommandPtr = std::unique_ptr<Command>;

            /*!
                @brief      Function with which to create a subcommand instance

                The function's argument is the list of commandline arguments not
                parsed by the encapsulating command
            */
            using SubcommandCreator = std::function<CommandPtr(int, char const* const*)>;

            /*!
                @brief      Dictionary for looking up subcommand create functions
                            by name
            */
            using SubcommandCreators = std::map<std::string, SubcommandCreator>;

            Command(Command const&) = delete;

            Command(Command&&) = delete;

            virtual ~Command() = default;

            auto operator=(Command const&) -> Command& = delete;

            auto operator=(Command&&) -> Command& = delete;

            auto run() noexcept -> int;

            [[nodiscard]] auto argument_parsed(std::string const& name) const -> bool;

            /*!
                @brief      Return the argument value of argument @a name
            */
            template<typename T>
            auto argument(std::string const& name) const -> T
            {
                return _parse_result[name].as<T>();
            }

        protected:

            Command(cxxopts::Options&& options, int argc, char const* const* argv);

            Command(
                cxxopts::Options&& options,
                int argc,
                char const* const* argv,
                SubcommandCreators&& subcommand_creators);

            void print_info_message(std::string const& message) const;

            void print_error_message(std::string const& message) const;

            virtual auto run_implementation() -> int;

        private:

            void handle_info(std::string const& message);

            void handle_error(std::string const& message);

            //! Stream to write informational message to
            std::ostream& _info_stream;

            //! Stream to write error messages to
            std::ostream& _error_stream;

            cxxopts::ParseResult _parse_result;

            //! Dictionary with subcommand creators by name
            SubcommandCreators _subcommand_creators;

            //! Subcommand selected for handling of user's request
            CommandPtr _sub_command;
    };

}  // namespace lue::utility
