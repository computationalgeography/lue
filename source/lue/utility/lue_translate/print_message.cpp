#include "lue/utility/lue_translate/print_message.h"


namespace lue {

void print_message(
    std::ostream& stream,
    std::string const& message)
{
    stream << message << std::endl;
}


void print_info_message(
    bool const verbose,
    std::ostream& stream,
    std::string const& message)
{
    if(verbose) {
        print_message(stream, "info: " + message);
    }
}


void print_error_message(
    std::ostream& stream,
    std::string const& message)
{
    print_message(stream, "error: " + message);
}

} // namespace lue
