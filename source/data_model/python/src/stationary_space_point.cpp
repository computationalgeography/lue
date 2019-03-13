#include "python_extension.hpp"
#include "lue/info/space/stationary_space_point.hpp"
#include <pybind11/pybind11.h>
#include <fmt/format.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace {

static std::string formal_string_representation(
    StationarySpacePoint const& point)
{
    return fmt::format(
            "StationarySpacePoint(pathname='{}')",
            point.id().pathname()
        );
}


static std::string informal_string_representation(
    StationarySpacePoint const& point)
{
    return fmt::format(
            "{}\n"
            "    nr_points: {}",
            formal_string_representation(point),
            point.nr_points()
        );
}

}  // Anonymous namespace

void init_stationary_space_point(
    py::module& module)
{

    py::class_<StationarySpacePoint, same_shape::Value>(
        module,
        "StationarySpacePoint",
        R"(
    TODO
)")

        .def(
            "__repr__",
            [](StationarySpacePoint const& point) {
                return formal_string_representation(point);
            }
        )

        .def(
            "__str__",
            [](StationarySpacePoint const& point) {
                return informal_string_representation(point);
            }
        )

        ;

}

}  // namespace lue
