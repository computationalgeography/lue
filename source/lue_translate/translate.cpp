#include "lue_translate/translate.h"
#include "lue_translate/dot.h"
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <exception>


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
namespace {

using LUEDatasetPtr = std::unique_ptr<Dataset>;


LUEDatasetPtr try_open_lue_dataset_for_read(
    std::string const& dataset_name)
{
    LUEDatasetPtr result;

    try {
        result = std::make_unique<Dataset>(dataset_name, H5F_ACC_RDONLY);
    }
    catch(...) {
    }

    return result;
}

}  // Anonymous namespace


namespace bfs = boost::filesystem;


Translate::Translate(
    int argc,
    char* argv[],
    std::string const& usage)

    : Application(argc, argv, usage)

{
    // GDALAllRegister();
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


    if(auto lue_dataset = try_open_lue_dataset_for_read(input_dataset_name)) {
        // Input is a dataset that can be read by LUE.
        // Assume we need to convert from the LUE format to some other format.

        if(bfs::path(output_dataset_name).extension() == ".dot") {
            // Create a Graphviz DOT graph of the dataset.
            translate_lue_dataset_to_dot(*lue_dataset, output_dataset_name);
        }
        else {
            throw std::runtime_error("translation to " + output_dataset_name +
                " is not supported");
        }
    }
    else {
        // Input is a dataset that cannot be read  by LUE.
        // Assume we need to convert from some other format to the LUE format.

        // First, try using the GDAL API.



        throw std::runtime_error("translation from " + input_dataset_name +
            " is not supported (does it exist?)");
    }
}

}  // namespace lue
