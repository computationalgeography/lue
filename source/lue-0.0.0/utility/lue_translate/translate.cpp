#include "lue/utility/lue_translate/translate.h"
#include "lue/utility/lue_translate/gdal.h"
#include <boost/format.hpp>
#include "lue/utility/lue_translate/time/from_string.h"
// #include "lue/utility/lue_translate/geometry.h"  // TODO Remove


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


namespace lue {

Translate::Translate(
    int argc,
    char* argv[],
    std::string const& usage)

    : Application(argc, argv, usage)

{
    GDALAllRegister();
}


void Translate::print_format(
    std::string const& dataset_name,
    std::string const& format)
{
    print_verbose_message((boost::format("%1% format: %2%")
        % dataset_name
        % format).str());
}


void Translate::run_implementation()
{
    std::string const input_dataset_name = argument<std::string>("<input>");
    std::string const output_dataset_name = argument<std::string>("<output>");
    bool const stack_passed = argument_passed("--start");


    if(!stack_passed) {

        print_verbose_message("translate dataset");

        if(auto gdal_dataset = try_open_gdal_raster_dataset_for_read(
                input_dataset_name)) {

            // Input is a dataset that can be read by GDAL. Assume we are
            // translating from a GDAL supported format to the LUE format.

            print_format(input_dataset_name,
                gdal_dataset->GetDriverName());

            translate_gdal_raster_to_lue(*gdal_dataset,
                input_dataset_name, output_dataset_name);
        }
        else if(auto gdal_dataset = try_open_gdal_track_dataset_for_read(
                input_dataset_name)) {

            // Input is a dataset that can be read by GDAL. The dataset
            // appears to contain a GPS track which we can import as a
            // mobile agent.

            print_format(input_dataset_name, gdal_dataset->GetDriverName());

            translate_gdal_track_to_lue(*gdal_dataset,
                input_dataset_name, output_dataset_name);
        }
        else {
            throw std::runtime_error("dataset " + input_dataset_name +
                " cannot be opened");
        }

    }
    else {

        print_verbose_message("translate stack dataset");

        if(auto gdal_dataset = try_open_gdal_raster_stack_for_read(
                input_dataset_name)) {

            // Input is the name of a stack of datasets that can be read by
            // GDAL.

            print_format(input_dataset_name, gdal_dataset->GetDriverName());

            bool parse_succeeded;
            MonthTimePoint time_point;
            MonthDuration duration;
            std::string time_point_string = argument<std::string>("--start");
            std::string duration_string = argument<std::string>("--cell");
            std::tie(parse_succeeded, time_point, duration) = parse_months(
                time_point_string, duration_string);

            if(!parse_succeeded) {
                throw std::runtime_error("time point " + time_point_string +
                    " and/or duration " + duration_string +
                    " cannot be parsed");
            }

            translate_gdal_raster_stack_to_lue(*gdal_dataset,
                input_dataset_name, time_point, duration,
                output_dataset_name);
        }
        else if(auto gdal_dataset = try_open_gdal_feature_stack_for_read(
                input_dataset_name)) {

            print_format(input_dataset_name, gdal_dataset->GetDriverName());

            bool parse_succeeded;
            MonthTimePoint time_point;
            MonthDuration duration;
            std::string time_point_string = argument<std::string>("--start");
            std::string duration_string = argument<std::string>("--cell");
            std::tie(parse_succeeded, time_point, duration) = parse_months(
                time_point_string, duration_string);

            if(!parse_succeeded) {
                throw std::runtime_error("time point " + time_point_string +
                    " and/or duration " + duration_string +
                    " cannot be parsed");
            }

            translate_gdal_feature_stack_to_lue(*gdal_dataset,
                input_dataset_name, time_point, duration,
                output_dataset_name);
        }
        else {
            throw std::runtime_error("stack " + input_dataset_name +
                " cannot be opened");
        }

   }
}

}  // namespace lue
