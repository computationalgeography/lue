#include "lue/view/application.hpp"
#include "lue/hdf5.hpp"
#include "lue/imgui.hpp"
#include <fmt/format.h>


// - Visualize property values
//     - time domain
//         - time boxes
//         - time cells
//         - time points
//     - space domain
//         - space points
//             - points map
//         - space boxes
//             - boxes map
//     - id
//     - properties
//         - time series graph
//         - raster map
//         - scalars table
// - Interface
//     - Navigator, showing an overview (layout, hierarchy) of a dataset
//     - View, visualizing a piece of information
//         - time domain
//         - space domain
//         - property
// - Make it possible to visualize multiple pieces of information at the
//   same time. If they are part of the same property-set, set up
//   a link/cursor/... E.g. selecting a space point should show the
//   time series graph of a selected property for that point.
//   Also set up links between stuff in the same phenomenon. Set up links
//   between as much stuff as possible.


namespace lue::view {
    namespace {

        std::string const usage = fmt::format(
            R"(
View LUE datasets

usage:
    {0}
    {0} <dataset>...
    {0} (-h | --help) | --version

options:
    -h --help   Show this screen
    --version   Show version
)",
            "lue_view");

    }  // Anonymous namespace


    Application::Application(std::vector<std::string> const& arguments):

        utility::Application{usage, arguments}

    {
        // Turn off error stack traversal. The default functions prints
        // lots of messages we usually don't care about.
        H5Eset_auto(H5E_DEFAULT, nullptr, nullptr);
    }

}  // namespace lue::view
