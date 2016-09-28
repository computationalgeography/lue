#include <boost/format.hpp>
#include <docopt/docopt.h>
#include "lue/utility/lue_translate/gdal.h"
// #include "lue/utility/lue_translate/geometry.h"  // TODO Remove
#include "lue/utility/lue_translate/stopwatch.h"
#include "lue/utility/lue_translate/print_message.h"
#include "lue/utility/lue_translate/time/from_string.h"
#include <hdf5.h>


// - Support creation of new dataset.
// - Support addition to existing dataset.
// - gdal2lue, lue2gdal
//     - Accept individual rasters.
//     - Accept stack names whose name is formatted according
//         to classic PCRaster convention and modern Aguila
//         convention.
//         - Browse for slices.
//         - Requires a start time point and time cell duration.
//         - Accept missing slices.
//     - Copy all metadata too.
//     - Requires phenomenon name (name of input dataset?).
// - pcraster2lue, lue2pcraster
// - netcdf2lue, lue2netcdf
// - lue2vtk
// - ...


namespace {

static char const usage[] = R"(lue_translate

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


void print_arguments(
    bool const verbose,
    std::ostream& stream,
    std::map<std::string, docopt::value> const& arguments)
{
    lue::print_info_message(verbose, stream, "commandline arguments:");

    for(auto const& pair: arguments) {
        lue::print_info_message(verbose, stream,
            (boost::format("    %1%: %2%")
                % pair.first
                % pair.second).str());
    }
}


void print_format(
    bool const verbose,
    std::ostream& stream,
    std::string const& dataset_name,
    std::string const& format)
{
    lue::print_info_message(verbose, stream,
        (boost::format("%1% format: %2%")
            % dataset_name
            % format).str());
}


void translate(
    int const argc,
    char const** argv,
    std::ostream& info_stream)
{
    using namespace lue;

    std::map<std::string, docopt::value> arguments = docopt::docopt(
        usage, { argv + 1, argv + argc }, true, "0.0.0");

    std::string const input_dataset_name =
        arguments["<input>"].asString();
    std::string const output_dataset_name =
        arguments["<output>"].asString();
    bool const verbose = arguments["--verbose"].asBool() ||
        arguments["--verbose_dev"].asBool();
    bool const verbose_dev = arguments["--verbose_dev"].asBool();
    bool const stack_passed = static_cast<bool>(
        arguments.find("--start")->second);

    print_arguments(verbose, info_stream, arguments);


    if(!verbose_dev) {
        // Turn off error stack traversal. The default functions prints
        // lots of messages we normally don't care about.
        H5Eset_auto1(NULL, NULL);
    }


    lue::Stopwatch stopwatch;
    stopwatch.start();


    if(!stack_passed) {

        lue::print_info_message(verbose, info_stream, "translate dataset");

        if(auto gdal_dataset = lue::try_open_gdal_raster_dataset_for_read(
                input_dataset_name)) {

            // Input is a dataset that can be read by GDAL. Assume we are
            // translating from a GDAL supported format to the LUE format.

            print_format(verbose, info_stream, input_dataset_name,
                gdal_dataset->GetDriverName());

            lue::translate_gdal_raster_to_lue(*gdal_dataset,
                input_dataset_name, output_dataset_name);
        }
        else if(auto gdal_dataset = lue::try_open_gdal_track_dataset_for_read(
                input_dataset_name)) {

            // Input is a dataset that can be read by GDAL. The dataset
            // appears to contain a GPS track which we can import as a
            // mobile agent.

            print_format(verbose, info_stream, input_dataset_name,
                gdal_dataset->GetDriverName());

            lue::translate_gdal_track_to_lue(*gdal_dataset,
                input_dataset_name, output_dataset_name);
        }
        else {
            throw std::runtime_error("dataset " + input_dataset_name +
                " cannot be opened");
        }

    }
    else {

        lue::print_info_message(verbose, info_stream,
            "translate stack dataset");

        if(auto gdal_dataset = lue::try_open_gdal_raster_stack_for_read(
                input_dataset_name)) {

            // Input is the name of a stack of datasets that can be read by
            // GDAL.

            print_format(verbose, info_stream, input_dataset_name,
                gdal_dataset->GetDriverName());

            bool parse_succeeded;
            MonthTimePoint time_point;
            MonthDuration duration;
            std::string time_point_string = arguments["--start"].asString();
            std::string duration_string = arguments["--cell"].asString();
            std::tie(parse_succeeded, time_point, duration) = parse_months(
                time_point_string, duration_string);

            if(!parse_succeeded) {
                throw std::runtime_error("time point " + time_point_string +
                    " and/or duration " + duration_string +
                    " cannot be parsed");
            }

            lue::translate_gdal_raster_stack_to_lue(*gdal_dataset,
                input_dataset_name, time_point, duration,
                output_dataset_name);
        }
        else if(auto gdal_dataset = lue::try_open_gdal_feature_stack_for_read(
                input_dataset_name)) {

            print_format(verbose, info_stream, input_dataset_name,
                gdal_dataset->GetDriverName());

            bool parse_succeeded;
            MonthTimePoint time_point;
            MonthDuration duration;
            std::string time_point_string = arguments["--start"].asString();
            std::string duration_string = arguments["--cell"].asString();
            std::tie(parse_succeeded, time_point, duration) = parse_months(
                time_point_string, duration_string);

            if(!parse_succeeded) {
                throw std::runtime_error("time point " + time_point_string +
                    " and/or duration " + duration_string +
                    " cannot be parsed");
            }

            lue::translate_gdal_feature_stack_to_lue(*gdal_dataset,
                input_dataset_name, time_point, duration,
                output_dataset_name);
        }
        else {
            throw std::runtime_error("stack " + input_dataset_name +
                " cannot be opened");
        }

    }

    stopwatch.stop();
    lue::print_info_message(verbose, info_stream, "finished at " +
        to_string(stopwatch.end()));
    lue::print_info_message(verbose, info_stream, "elapsed time: " +
        std::to_string(stopwatch.elapsed_seconds()) + "s");
}


// void MyErrorHandler(
//     CPLErr /* eErrClass */,
//     int err_no,
//     char const* msg)
// {
//     std::cout << msg << std::endl;
//     assert(false);
// }

} // Anonymous namespace


int main(
    int const argc,
    char const** argv)
{
    std::ostream& info_stream = std::cout;
    std::ostream& error_stream = std::cerr;

    GDALAllRegister();
    // CPLSetErrorHandler(MyErrorHandler);

    int status = EXIT_FAILURE;

    try {
        translate(argc, argv, info_stream);
        status = EXIT_SUCCESS;
    }
    catch(std::bad_alloc const& exception) {
        lue::print_error_message(error_stream, "not enough memory");
    }
    catch(std::exception const& exception) {
        lue::print_error_message(error_stream, exception.what());
    }

    return status;
}
