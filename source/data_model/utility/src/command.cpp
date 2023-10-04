#include "lue/utility/command.hpp"
#include "lue/utility/print_message.hpp"
// #include "lue/utility/stopwatch.hpp"
#include "lue/git.hpp"
#include "lue/version.hpp"
#include <fmt/format.h>
#include <cassert>
#include <iostream>


namespace lue {
    namespace utility {

        template<>
        std::string Command::argument<std::string>(std::string const& name) const
        {
            assert(argument_parsed(name));
            return _arguments.at(name).asString();
        }


        template<>
        std::vector<std::string> Command::argument<std::vector<std::string>>(std::string const& name) const
        {
            assert(argument_parsed(name));
            return _arguments.at(name).asStringList();
        }


        template<>
        bool Command::argument<bool>(std::string const& name) const
        {
            assert(argument_parsed(name));
            return _arguments.at(name).asBool();
        }


        /*!
            @brief      Construct an instance without subcommands
            @param      usage Usage string
            @param      arguments Commandline arguments
            @exception  In case of a commandline parsing error, the program will exit
                        with exit code -1
        */
        Command::Command(std::string const& usage, std::vector<std::string> const& arguments)

            :
            Command(usage, arguments, SubcommandCreators{})

        {
            // print_arguments();
        }


        /*!
            @brief      Construct an instance with subcommands
            @param      usage Usage string
            @param      arguments Commandline arguments
            @param      subcommand_creators Collection of creator functions to use
                        when a subcommand is requested
            @exception  In case of a commandline parsing error, the program will exit
                        with exit code -1

            It is assumed that if a subcommand is requested, an argument named
            `<arguments>` contains all arguments that need to be passed into the
            subcommand creator function.
        */
        Command::Command(
            std::string const& usage,
            std::vector<std::string> const& arguments,
            SubcommandCreators const& subcommand_creators)

            :
            _info_stream(std::cout),
            _error_stream(std::cerr),
            _arguments(docopt::docopt(
                usage, arguments, true, fmt::format("{} (build {})", version(), short_sha1()), true)),
            _subcommand_creators{subcommand_creators},
            _sub_command()

        {
            // print_arguments();

            for (auto const& sub_command : _subcommand_creators)
            {
                auto const command_name = sub_command.first;
                assert(argument_parsed(command_name));

                if (argument<bool>(command_name))
                {
                    assert(argument_parsed("<arguments>"));
                    auto const arguments_ = argument<std::vector<std::string>>("<arguments>");

                    _sub_command = _subcommand_creators[command_name](arguments_);
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


        // void Command::print_arguments()
        // {
        //     print_info_message("commandline arguments:");
        //
        //     for(auto const& pair: _arguments) {
        //         print_info_message((boost::format("    %1%: %2%")
        //             % pair.first
        //             % pair.second).str());
        //     }
        // }


        /*!
            @brief      Test whether argument @a name is parsed
            @warning    An argument with name @a name must be part of the usage of
                        the command
        */
        bool Command::argument_parsed(std::string const& name) const
        {
            assert(_arguments.find(name) != _arguments.end());

            return static_cast<bool>(_arguments.find(name)->second);
        }


        /*!
            @brief      Do whatever it takes to perform the command's action
            @return     In case no exception is raised, this function should return
                        an integer status code, e.g.: EXIT_FAILURE or EXIT_SUCCESS
            @warning    This function must be overridden by a specialization or
                        a subcommand
        */
        int Command::run_implementation()
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
        int Command::run() noexcept
        {
            int status = EXIT_FAILURE;

            try
            {

                // Stopwatch stopwatch;
                // stopwatch.start();

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

                // stopwatch.stop();
                // print_info_message("finished at: " + to_string(stopwatch.end()));
                // print_info_message("elapsed time: " + std::to_string(
                //     stopwatch.elapsed_seconds()) + "s");
            }
            catch (...)
            {
                print_error_message("unexpected error");
                status = EXIT_FAILURE;
            }

            return status;
        }

    }  // namespace utility
}  // namespace lue
