#include "lue/translate/translate.hpp"
#include "lue/gdal.hpp"
#include "lue/translate/command.hpp"
#include <format>
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


namespace lue::utility {

    Translate::Translate(int const argc, char const* const* argv):
        Application{
            [] -> cxxopts::Options
            {
                cxxopts::Options options{
                    "lue_translate", "Translate data into or from the LUE dataset format"};

                options.positional_help("<convert | import | export>");
                options.add_options()
                    // clang-format off
                    ("h,help", "Show usage")
                    ("v,version", "Show version")
                    ("command", "Command to use", cxxopts::value<std::string>())
                    // clang-format on
                    ;
                options.parse_positional({"command"});
                options.allow_unrecognised_options();

                return options;
            }(),
            argc,
            argv,
            {
                {Import::name, Import::command},
                {Export::name, Export::command},
            }}

    {
        // Turn off error stack traversal. The default functions prints
        // lots of messages we usually don't care about.
        H5Eset_auto(H5E_DEFAULT, nullptr, nullptr);

        gdal::register_gdal_drivers();
    }

}  // namespace lue::utility
