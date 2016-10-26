#include "lue/utility/lue_dump/dump.h"


namespace {

std::string const usage = R"(lue_dump

usage:
    lue_validate [--verbose] [--verbose_dev] [--format=<format>] <input>
    lue_validate (-h | --help)
    lue_validate --version

options:
    -h --help             Show this screen
    --verbose             Show info messages
    --verbose_dev         Show info messages for developer (implies --verbose)
    --version             Show version
    --format=<format>     Output format [default: txt]
)";

} // Anonymous namespace


int main(
    int argc,
    char* argv[])
{
    return lue::Dump(argc, argv, usage).run();
}
