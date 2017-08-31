#include "lue/utility/command.hpp"
#include "lue/utility/print_message.hpp"
#include "lue/utility/stopwatch.hpp"
#include "lue/configure.hpp"
#include <boost/format.hpp>
#include <cassert>
#include <iostream>


namespace lue {
namespace utility {

template<>
std::string Command::argument<std::string>(
    std::string const& name) const
{
    assert(argument_passed(name));
    return _arguments.at(name).asString();
}


template<>
std::vector<std::string> Command::argument<std::vector<std::string>>(
    std::string const& name) const
{
    assert(argument_passed(name));
    return _arguments.at(name).asStringList();
}


template<>
bool Command::argument<bool>(
    std::string const& name) const
{
    assert(argument_passed(name));
    return _arguments.at(name).asBool();
}


Command::Command(
    std::string const& usage,
    std::vector<std::string> const& arguments)

    : Command(usage, arguments, SubCommands{})

{
    // print_arguments();
}


Command::Command(
    std::string const& usage,
    std::vector<std::string> const& arguments,
    SubCommands const& sub_commands)

    : _info_stream(std::cout),
      _error_stream(std::cerr),
      _arguments(docopt::docopt(usage, arguments, true, LUE_VERSION, true)),
      _sub_commands{sub_commands},
      _sub_command()

{
    // print_arguments();

    for(auto const& sub_command: _sub_commands) {
        auto const command_name = sub_command.first;
        assert(argument_passed(command_name));

        if(argument<bool>(command_name)) {
            assert(argument_passed("<arguments>"));
            auto const arguments = argument<std::vector<std::string>>(
                "<arguments>");

            _sub_command = _sub_commands[command_name](arguments);
        }
    }
}


void Command::print_info_message(
    std::string const& message) const
{
    lue::utility::print_info_message(_info_stream, message);
}


void Command::print_error_message(
    std::string const& message) const
{
    lue::utility::print_error_message(_error_stream, message);
}


// void Command::print_verbose_message(
//     std::string const& message) const
// {
//     lue::utility::print_info_message(_verbose, _info_stream, message);
// }


void Command::print_arguments()
{
    print_info_message("commandline arguments:");

    for(auto const& pair: _arguments) {
        print_info_message((boost::format("    %1%: %2%")
            % pair.first
            % pair.second).str());
    }
}


bool Command::argument_passed(
    std::string const& name) const
{
    assert(_arguments.find(name) != _arguments.end());

    return static_cast<bool>(_arguments.find(name)->second);
}


void Command::run_implementation()
{
    // Either:
    // - This method should be overridden in a specialization
    // - A sub command should have been selected (while parsing the command
    //   line) and its run_implementation() called
    assert(false);
}


int Command::run() noexcept
{
    int status = EXIT_FAILURE;

    try {
        Stopwatch stopwatch;
        stopwatch.start();

        try {
            if(_sub_command) {
                _sub_command->run_implementation();
            }
            else {
                run_implementation();
            }

            status = EXIT_SUCCESS;
        }
        catch(std::bad_alloc const& exception) {
            print_error_message("not enough memory");
        }
        catch(std::exception const& exception) {
            print_error_message(exception.what());
        }

        stopwatch.stop();
        // print_verbose_message("finished at: " + to_string(stopwatch.end()));
        // print_verbose_message("elapsed time: " + std::to_string(
        //     stopwatch.elapsed_seconds()) + "s");

    }
    catch(...) {
        print_error_message("unknown error");
    }

    // print_verbose_message("exit status: " + std::to_string(status));

    return status;
}

}  // namespace utility
}  // namespace lue
