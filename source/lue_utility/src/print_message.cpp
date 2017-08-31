#include "lue/utility/print_message.hpp"


namespace lue {
namespace utility {

void print_message(
    std::ostream& stream,
    std::string const& message)
{
    stream << message << std::endl;
}


void print_info_message(
    std::ostream& stream,
    std::string const& message)
{
    print_message(stream, /* "info: " + */ message);
}


void print_info_message(
    bool const verbose,
    std::ostream& stream,
    std::string const& message)
{
    if(verbose) {
        print_info_message(stream, message);
    }
}


void print_error_message(
    std::ostream& stream,
    std::string const& message)
{
    print_message(stream, "error: " + message);
}

} // namespace utility
} // namespace lue
