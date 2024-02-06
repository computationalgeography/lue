#include "lue/translate/command/export.hpp"
#include "lue/translate/format.hpp"
#include "lue/validate.hpp"
#include <exception>
#include <filesystem>


namespace lue {
    namespace utility {
        namespace {

            std::string const usage = R"(
Translate data from the LUE dataset format

usage:
    export [-m <name>] <input> <output>
    export (-h | --help)

arguments:
    <input>     Input dataset
    <output>    Output dataset

options:
    -h --help   Show this screen
    -m <name> --meta=<name>  File containing metadata to use during import
)";

        }  // Anonymous namespace


        std::string const Export::name = "export";


        Command::CommandPtr Export::command(std::vector<std::string> const& arguments)
        {
            return std::make_unique<Export>(arguments);
        }


        Export::Export(std::vector<std::string> const& arguments)

            :
            Command(usage, arguments)

        {
        }


        int Export::run_implementation()
        {
            auto const input_dataset_name = argument<std::string>("<input>");
            auto const output_dataset_name = argument<std::string>("<output>");

            bool const metadata_passed = argument_parsed("--meta");

            auto const metadata = metadata_passed ? Metadata(argument<std::string>("--meta")) : Metadata();


            auto lue_dataset = try_open_lue_dataset_for_read(input_dataset_name);

            if (!lue_dataset)
            {
                throw std::runtime_error("cannot open LUE dataset " + input_dataset_name);
            }


            // First see if the input is valid
            {
                hdf5::Issues issues;
                data_model::validate(*lue_dataset, issues);

                if (issues.errors_found())
                {
                    throw std::runtime_error(data_model::message(issues));
                }

                if (issues.warnings_found())
                {
                    // Let's assume nothing bad will happen...
                    print_info_message(data_model::message(issues));
                }
            }

            // Input is a dataset that can be read by LUE.
            // We need to convert from the LUE format to some other format.

            if (std::filesystem::path(output_dataset_name).extension() == ".dot")
            {
                // Create a Graphviz DOT graph of the dataset.
                translate_lue_dataset_to_dot(*lue_dataset, output_dataset_name, metadata);
            }
            // else if(std::filesystem::path(output_dataset_name).extension() == ".csv") {
            //    // Create a CSV file of the dataset.
            //    translate_lue_dataset_to_csv(
            //        *lue_dataset, output_dataset_name, metadata);
            // }
            else if (std::filesystem::path(output_dataset_name).extension() == ".shp")
            {
                // Write information from the dataset to one or more Shapefiles
                translate_lue_dataset_to_shapefile(*lue_dataset, output_dataset_name, metadata);
            }
            else if (std::filesystem::path(output_dataset_name).extension() == ".tif")
            {
                // Write information from the dataset to one or more rasters
                translate_lue_dataset_to_gdal_raster(*lue_dataset, output_dataset_name, metadata);
            }
            // else if(std::filesystem::path(output_dataset_name).extension() == ".vtk") {
            //     // Create a VTK file of the dataset.
            //     translate_lue_dataset_to_vtk(
            //         *lue_dataset, output_dataset_name, metadata);
            // }
            else
            {
                throw std::runtime_error("translation to " + output_dataset_name + " is not supported");
            }


            return EXIT_SUCCESS;
        }

    }  // namespace utility
}  // namespace lue
