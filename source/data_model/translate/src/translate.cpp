#include "lue/translate/translate.hpp"
#include "lue/translate/command.hpp"
#include <gdal_priv.h>
#include <hdf5.h>
#include <fmt/format.h>


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

std::string const usage = fmt::format(R"(
Translate data into or from the LUE dataset format

usage:
    {0} (import | export) [<arguments>...]
    {0} (-h | --help) | --version

options:
    -h --help             Show this screen
    --version             Show version

See '{0} <command> --help' for more information on a command.
)", "lue_translate");


// std::string const usage = R"(lue_translate
//
// usage:
//     lue_translate [--verbose] <input> <output>
//     lue_translate [--verbose] --start=<time_point>
//         --cell=<duration> <input> <output>
//     lue_translate (-h | --help) | --version
//
// options:
//     -h --help             Show this screen
//     --verbose             Show info messages
//     --version             Show version
//     --start=<time_point>  Time point of first slice
//     --cell=<duration>     Duration of time step
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

} // Anonymous namespace


namespace lue {
namespace utility {

Translate::Translate(
    std::vector<std::string> const& arguments)
:
    Application(usage, arguments, {
          {Import::name, Import::command},
          {Export::name, Export::command},
      })

{
    // Turn off error stack traversal. The default functions prints
    // lots of messages we usually don't care about.
    H5Eset_auto(H5E_DEFAULT, nullptr, nullptr);

    GDALAllRegister();
}


// void Translate::print_format(
//     std::string const& dataset_name,
//     std::string const& format)
// {
//     print_verbose_message((boost::format("%1% format: %2%")
//         % dataset_name
//         % format).str());
// }

}  // namespace utility
}  // namespace lue
