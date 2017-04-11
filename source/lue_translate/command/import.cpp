#include "lue_translate/command/import.h"
#include <cassert>
#include <string>


namespace lue {
namespace utility {
namespace {

std::string const usage = R"(
Translate data into the LUE dataset format

usage:
    import <input> <output>
    import (-h | --help)

options:
    -h --help   Show this screen
    <input>     Input dataset
    <output>    Output dataset
)";

}  // Anonymous namespace


Import::Import(
    int argc,
    char* argv[])

    : Command(argc, argv, usage)

{
}


void Import::run_implementation()
{
}

}  // namespace utility
}  // namespace lue
