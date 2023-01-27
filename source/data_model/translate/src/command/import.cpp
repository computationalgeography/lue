#include "lue/translate/command/import.hpp"
#include "lue/translate/format.hpp"
#include "lue/validate.hpp"
#include <exception>
#include <filesystem>


namespace lue {
namespace utility {
namespace {

std::string const usage = R"(
Translate data into the LUE dataset format

usage:
    import [-m <name>] [--add] [--skip-validate] <output> <inputs>...
    import (-h | --help)

arguments:
    <inputs>    Input dataset(s)
    <output>    Output dataset

options:
    -h --help   Show this screen
    -m <name> --meta=<name>  File containing metadata to use during import
    --add       Add data to the output dataset instead of overwriting it
    --skip-validate  Skip validating the resulting LUE dataset
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


int Import::run_implementation()
{
    auto const input_dataset_names =
        argument<std::vector<std::string>>("<inputs>");
    auto const output_dataset_name = argument<std::string>("<output>");

    bool const metadata_passed = argument_parsed("--meta");
    bool const add_passed = argument<bool>("--add");
    bool const skip_validate = argument<bool>("--skip-validate");
    // bool const stack_passed = argument_parsed("--start");

    auto const metadata = metadata_passed
        ? Metadata(argument<std::string>("--meta"))
        : Metadata();

    auto const first_input_dataset_name = input_dataset_names[0];

    // if(input_dataset_names.size() == 1) {
    //     auto const input_dataset_name = first_input_dataset_name;

    //     if(try_open_json_file_for_read(first_input_dataset_name)) {
    //     }

    // }

    // if(try_open_gdal_raster_stack_dataset_for_read(first_input_dataset_name)) {

    //     // First input is a dataset that can be read by GDAL.
    //     // We need to convert from a GDAL format to the LUE format.

    //     translate_gdal_raster_stack_dataset_to_lue(
    //         input_dataset_names, output_dataset_name, metadata);

    // }
    // else
    if(try_open_gdal_raster_dataset_for_read(first_input_dataset_name)) {

        // First input is a dataset that can be read by GDAL.
        // We need to convert from a GDAL format to the LUE format.
        translate_gdal_raster_dataset_to_lue(
            input_dataset_names, output_dataset_name,
            add_passed, metadata);
    }

    // Support import of various file formats into a single lue dataset
    else if(std::filesystem::path(first_input_dataset_name).extension() == ".json") {
        assert(input_dataset_names.size() == 1);
        translate_json_to_lue(
            first_input_dataset_name, output_dataset_name,
            add_passed, metadata);
    }
    else {
        throw std::runtime_error(
            "translation from " + first_input_dataset_name +
            " is not supported (does it exist?)");
    }

    if(!skip_validate) {
        // Verify that the dataset is usable
        data_model::assert_is_valid(output_dataset_name);
    }

    return EXIT_SUCCESS;
}

}  // namespace utility
}  // namespace lue
