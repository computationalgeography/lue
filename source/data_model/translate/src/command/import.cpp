#include "lue/translate/command/import.hpp"
#include "lue/translate/format.hpp"
#include "lue/validate.hpp"
#include <exception>
#include <filesystem>


namespace lue::utility {
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
    }  // Anonymous namespace


    std::string const Import::name = "import";


    Command::CommandPtr Import::command(
        std::vector<std::string> const& arguments)
    {
        return std::make_unique<Import>(arguments);
    }


    Import::Import(
        std::vector<std::string> const& arguments):

        Command(usage, arguments)

    {
    }


    int Import::run_implementation()
    {
        auto const input_dataset_names{argument<std::vector<std::string>>("<inputs>")};
        auto const output_dataset_name{argument<std::string>("<output>")};

        bool const metadata_passed{argument_parsed("--meta")};
        auto const metadata{metadata_passed
            ? Metadata{argument<std::string>("--meta")}
            : Metadata{}};

        bool append_data{argument<bool>("--add")};
        bool const skip_validate{argument<bool>("--skip-validate")};

        for(std::string const& input_dataset_name: input_dataset_names)
        {
            std::string const file_extension{std::filesystem::path(input_dataset_name).extension()};

            // TODO Grab metadata information for only the current input dataset

            if(file_extension == ".nc")
            {
                translate_netcdf4_dataset_to_lue(
                    input_dataset_name, output_dataset_name, append_data, metadata);
            }
            else if(try_open_gdal_raster_dataset_for_read(input_dataset_name))
            {
                translate_gdal_raster_dataset_to_lue(
                    input_dataset_names, output_dataset_name, append_data, metadata);
            }
            // // Support import of various file formats into a single lue dataset
            // else if(std::filesystem::path(input_dataset_name).extension() == ".json")
            // {
            //     assert(input_dataset_names.size() == 1);
            //     translate_json_to_lue(
            //         input_dataset_name, output_dataset_name,
            //         append_data, metadata);
            // }
            else
            {
                throw std::runtime_error(
                    "translation from " + input_dataset_name + " is not supported (does it exist?)");
            }

            // Once the first dataset is written to the output dataset, additional ones
            // must always be appended
            append_data = true;
        }

        if(!skip_validate)
        {
            // Verify that the dataset is usable
            data_model::assert_is_valid(output_dataset_name);
        }

        return EXIT_SUCCESS;
    }

}  // namespace lue::utility
