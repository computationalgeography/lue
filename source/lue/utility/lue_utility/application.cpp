#include "lue/utility/lue_utility/application.h"
#include "lue/utility/lue_utility/print_message.h"
#include "lue/utility/lue_utility/stopwatch.h"
#include <hdf5.h>
#include <boost/format.hpp>
#include <cassert>
#include <iostream>


namespace lue {
namespace {

// void MyErrorHandler(
//     CPLErr /* eErrClass */,
//     int err_no,
//     char const* msg)
// {
//     std::cout << msg << std::endl;
//     assert(false);
// }

}  // Anonymous namespace


template<>
std::string Application::argument<std::string>(
    std::string const& name) const
{
    assert(argument_passed(name));
    return _arguments.at(name).asString();
}


template<>
bool Application::argument<bool>(
    std::string const& name) const
{
    assert(argument_passed(name));
    return _arguments.at(name).asBool();
}


Application::Application(
    int argc,
    char* argv[],
    std::string const& usage)

    : _info_stream(std::cout),
      _error_stream(std::cerr),
      _arguments(docopt::docopt(usage, { argv + 1, argv + argc }, true,
          "0.0.0")),
      _verbose_dev(argument<bool>("--verbose_dev")),
      _verbose(_verbose_dev || argument<bool>("--verbose"))

{
    print_arguments();

    // // CPLSetErrorHandler(MyErrorHandler);

    if(!_verbose_dev) {
        // Turn off error stack traversal. The default functions prints
        // lots of messages we normally don't care about.
        H5Eset_auto1(NULL, NULL);
    }
}


void Application::print_info_message(
    std::string const& message) const
{
    lue::print_info_message(_info_stream, message);
}


void Application::print_error_message(
    std::string const& message) const
{
    lue::print_error_message(_error_stream, message);
}


void Application::print_verbose_message(
    std::string const& message) const
{
    lue::print_info_message(_verbose, _info_stream, message);
}


void Application::print_arguments()
{
    print_verbose_message("commandline arguments:");

    for(auto const& pair: _arguments) {
        print_verbose_message((boost::format("    %1%: %2%")
            % pair.first
            % pair.second).str());
    }
}


bool Application::argument_passed(
    std::string const& name) const
{
    return static_cast<bool>(_arguments.find(name)->second);
}


int Application::run() noexcept
{
    int status = EXIT_FAILURE;

    try {
        Stopwatch stopwatch;
        stopwatch.start();

        try {
            run_implementation();
        }
        catch(std::bad_alloc const& exception) {
            print_error_message("not enough memory");
        }
        catch(std::exception const& exception) {
            print_error_message(exception.what());
        }

        stopwatch.stop();
        print_verbose_message("finished at: " + to_string(stopwatch.end()));
        print_verbose_message("elapsed time: " + std::to_string(
            stopwatch.elapsed_seconds()) + "s");

        status = EXIT_SUCCESS;
    }
    catch(...) {
        print_error_message("unknown error");
    }

    print_verbose_message("exit status: " + std::to_string(status));

    return status;
}

}  // namespace lue
