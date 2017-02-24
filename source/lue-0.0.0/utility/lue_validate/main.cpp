#include "lue/utility/lue_validate/validate.h"


namespace {

std::string const usage = R"(lue_validate

usage:
    lue_validate [--verbose] [--verbose-dev] <input>...
    lue_validate (-h | --help)
    lue_validate --version

options:
    -h --help             Show this screen
    --verbose             Show info messages
    --verbose-dev         Show info messages for developer (implies --verbose)
    --version             Show version
)";

} // Anonymous namespace


int main(
    int argc,
    char* argv[])
{
    return lue::Validate(argc, argv, usage).run();
}
