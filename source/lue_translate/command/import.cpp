#include "lue_translate/command/import.h"
#include "lue_translate/format.h"
#include <boost/filesystem.hpp>
#include <exception>


namespace lue {
namespace utility {
namespace {

std::string const usage = R"(
Translate data into the LUE dataset format

usage:
    import [-m <name>] <input> <output>
    import (-h | --help)

arguments:
    <input>                Input dataset
    <output>               Output dataset

options:
    -h --help              Show this screen
    -m <name> --meta=<name>  File containing metadata to use during import
)";


// std::string const usage = R"(
// Translate data into the LUE dataset format
// 
// usage:
//     import [-m <name>] <input> <output>
//     import --start=<time_point> --cell=<duration> <input> <output>
//     import (-h | --help)
// 
// arguments:
//     <input>                Input dataset
//     <output>               Output dataset
// 
// options:
//     -h --help              Show this screen
//     -m <name> --meta=<name>  File containing metadata to use during import
//     --start=<time_point>   Time point of first slice
//     --cell=<duration>      Duration of time step
// 
// Time points must be formatted according to the ISO-8601 standard.
// 
// Durations must be formatted according to the ISO-8601 standard. A duration
// determines the resolution of the discretization, and the associated
// time point must 'match' this resolution: it makes no sense to provide
// a time point at a higher resolution than the duration. For example,
// given a duration of months (P1M), the time point must be given as YYYY-MM
// (and not YYYY-MM-DD, for example).
// )";

}  // Anonymous namespace


std::string const Import::name = "import";


Command::CommandPtr Import::command(
    std::vector<std::string> const& arguments)
{
    return std::make_unique<Import>(arguments);
}


Import::Import(
    std::vector<std::string> const& arguments)

    : Command(usage, arguments)

{
}


void Import::run_implementation()
{
    namespace bfs = boost::filesystem;

    std::string const input_dataset_name = argument<std::string>("<input>");
    std::string const output_dataset_name = argument<std::string>("<output>");

    bool const metadata_passed = argument_passed("--meta");
    // bool const stack_passed = argument_passed("--start");

    auto const metadata = metadata_passed
        ? Metadata(argument<std::string>("--meta"))
        : Metadata();

    // if(!stack_passed) {
        if(auto gdal_dataset = try_open_gdal_raster_dataset_for_read(
                input_dataset_name)) {

            // Input is a dataset that can be read by GDAL.
            // We need to convert from a GDAL format to the LUE format.

            translate_gdal_raster_dataset_to_lue(*gdal_dataset,
                output_dataset_name, metadata);
        }
        else {
            throw std::runtime_error(
                "translation from " + input_dataset_name +
                " is not supported (does it exist?)");
        }
    // }
    // else {
    //     throw std::runtime_error(
    //         "translation of temporal stacks is not supported yet");
    // }
}

}  // namespace utility
}  // namespace lue
