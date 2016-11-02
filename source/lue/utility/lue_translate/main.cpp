#include "lue/utility/lue_translate/translate.h"


namespace {

std::string const usage = R"(lue_translate

usage:
    lue_translate [--verbose] [--verbose_dev] <input> <output>
    lue_translate [--verbose] [--verbose_dev] --start=<time_point>
        --cell=<duration> <input> <output>
    lue_translate (-h | --help)
    lue_translate --version

options:
    -h --help             Show this screen
    --verbose             Show info messages
    --verbose_dev         Show info messages for developer (implies --verbose)
    --version             Show version
    --start=<time_point>  Time point of first slice
    --cell=<duration>     Duration of time step

Time points must be formatted according to the ISO-8601 standard.

Durations must be formatted according to the ISO-8601 standard. A duration
determines the resolution of the discretization, and the associated
time point must 'match' this resolution: it makes no sense to provide
a time point at a higher resolution than the duration. For example,
given a duration of months (P1M), the time point must be given as YYYY-MM
(and not YYYY-MM-DD, for example).
)";

} // Anonymous namespace


int main(
    int argc,
    char* argv[])
{
    return lue::Translate(argc, argv, usage).run();
}
