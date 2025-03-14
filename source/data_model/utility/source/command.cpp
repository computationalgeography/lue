#include "lue/utility/command.hpp"
#include "lue/git.hpp"
#include "lue/utility/print_message.hpp"
#include "lue/version.hpp"
#include <cassert>
#include <format>
#include <iostream>


namespace lue::utility {

    Command::Command(cxxopts::Options&& options, int const argc, char const* const* argv):

        Command(std::move(options), argc, argv, SubcommandCreators{})

    {
    }


    Command::Command(
        cxxopts::Options&& options,
        int const argc,
        char const* const* argv,
        SubcommandCreators&& subcommand_creators):

        _info_stream(std::cout),
        _error_stream(std::cerr),
        _subcommand_creators{std::move(subcommand_creators)}

    {
        try
        {
            _parse_result = options.parse(argc, argv);
        }
        catch (cxxopts::exceptions::exception const& exception)
        {
            handle_error(exception.what());
        }

        if (std::empty(_subcommand_creators) || !argument_parsed("command"))
        {
            // No command available or maybe we are parsing as part of a command. Check global options.
            if (_parse_result.count("help") != 0)
            {
                handle_info(options.help());
            }

            if (_parse_result.count("version") != 0)
            {
                handle_info(std::format("{} (build {})", version(), short_sha1()));
            }
        }

        if (!std::empty(_subcommand_creators))
        {
            // Commands available. Let's see if the user selected one.
            if (argument_parsed("command"))
            {
                auto const command_name_parsed = _parse_result["command"].as<std::string>();

                for (auto const& [command_name_supported, sub_command] : _subcommand_creators)
                {
                    if (command_name_parsed == command_name_supported)
                    {
                        assert(argc >= 2);
                        _sub_command = sub_command(argc - 1, argv + 1);
                        break;
                    }
                }

                if (!_sub_command)
                {
                    handle_error(std::format("Unknown command: {}", command_name_parsed));
                }
            }

            if (!_sub_command)
            {
                handle_error("Missing command");
            }
        }
    }


    /*!
        @brief      Write info message to info stream
    */
    void Command::print_info_message(std::string const& message) const
    {
        lue::utility::print_info_message(_info_stream, message);
    }


    /*!
        @brief      Write error message to error stream
    */
    void Command::print_error_message(std::string const& message) const
    {
        lue::utility::print_error_message(_error_stream, message);
    }


    void Command::handle_info(std::string const& message)
    {
        print_info_message(message);
        exit(EXIT_SUCCESS);
    }


    void Command::handle_error(std::string const& message)
    {
        print_error_message(std::format("{}, pass '--help' for more information", message));
        exit(EXIT_FAILURE);
    }


    /*!
        @brief      Test whether argument @a name is parsed
        @warning    An argument with name @a name must be part of the usage of
                    the command
    */
    auto Command::argument_parsed(std::string const& name) const -> bool
    {
        return _parse_result.count(name) > 0;
    }


    /*!
        @brief      Do whatever it takes to perform the command's action
        @return     In case no exception is raised, this function should return
                    an integer status code, e.g.: EXIT_FAILURE or EXIT_SUCCESS
        @warning    This function must be overridden by a specialization or
                    a subcommand
    */
    auto Command::run_implementation() -> int
    {
        assert(false);

        return EXIT_FAILURE;
    }


    /*!
        @brief      Do whatever it takes to perform the command's action
        @return     EXIT_FAILURE in case an exception is thrown by
                    run_implementation(), otherwise the status code returned
                    by run_implementation()
    */
    auto Command::run() noexcept -> int
    {
        int status = EXIT_FAILURE;

        try
        {
            try
            {
                status = _sub_command ? _sub_command->run_implementation() : run_implementation();
            }
            catch (std::bad_alloc const& exception)
            {
                print_error_message("not enough memory");
            }
            catch (std::exception const& exception)
            {
                print_error_message(exception.what());
            }
        }
        catch (...)
        {
            print_error_message("unexpected error");
        }

        return status;
    }

}  // namespace lue::utility
