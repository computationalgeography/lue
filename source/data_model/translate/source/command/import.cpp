#include "lue/translate/command/import.hpp"
#include "lue/translate/format.hpp"
#include "lue/validate.hpp"
#include <exception>
#include <filesystem>


namespace lue::utility {
    std::string const Import::name = "import";

    auto Import::command(int argc, char const* const* argv) -> CommandPtr
    {
        return std::make_unique<Import>(argc, argv);
    }


    Import::Import(int const argc, char const* const* argv):

        Command(
            []() -> cxxopts::Options
            {
                cxxopts::Options options{Import::name, "Translate data into the LUE dataset format"};
                options.add_options()("h,help", "Show usage")(
                    "m,meta", "File containing metadata to use during import", cxxopts::value<std::string>())(
                    "add",
                    "Add data to the output dataset instead of overwriting it",
                    cxxopts::value<bool>()->default_value("false"))(
                    "skip-validate",
                    "Skip validating the resulting LUE dataset",
                    cxxopts::value<bool>()->default_value("false"))(
                    "output", "Output dataset", cxxopts::value<std::string>())(
                    "input", "Input dataset(s)", cxxopts::value<std::vector<std::string>>());
                options.parse_positional({"output", "input"});
                options.positional_help("<output> <input>...");
                options.show_positional_help();
                return options;
            }(),
            argc,
            argv)

    {
    }


    auto Import::run_implementation() -> int
    {
        if (!argument_parsed("input"))
        {
            throw std::runtime_error("missing input");
        }

        if (!argument_parsed("output"))
        {
            throw std::runtime_error("missing output");
        }

        auto const input_dataset_names = argument<std::vector<std::string>>("input");
        auto const output_dataset_name = argument<std::string>("output");
        bool const add_data = argument<bool>("add");
        bool const skip_validate = argument<bool>("skip-validate");
        // bool const stack_passed = argument_parsed("--start");
        auto const metadata = argument_parsed("meta") ? Metadata(argument<std::string>("meta")) : Metadata();

        auto const& first_input_dataset_name = input_dataset_names[0];

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
        if (try_open_gdal_raster_dataset_for_read(first_input_dataset_name))
        {
            // First input is a dataset that can be read by GDAL.
            // We need to convert from a GDAL format to the LUE format.
            translate_gdal_raster_dataset_to_lue(
                input_dataset_names, output_dataset_name, add_data, metadata);
        }

        // Support import of various file formats into a single lue dataset
        else if (std::filesystem::path(first_input_dataset_name).extension() == ".json")
        {
            assert(input_dataset_names.size() == 1);
            translate_json_to_lue(first_input_dataset_name, output_dataset_name, add_data, metadata);
        }
        else
        {
            throw std::runtime_error(
                "translation from " + first_input_dataset_name + " is not supported (does it exist?)");
        }

        if (!skip_validate)
        {
            // Verify that the dataset is usable
            data_model::assert_is_valid(output_dataset_name);
        }

        return EXIT_SUCCESS;
    }

}  // namespace lue::utility
