#include "python_extension.hpp"
#include "lue/info/space/mobile_space_point.hpp"
#include <pybind11/pybind11.h>
#include <fmt/format.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace {

static std::string formal_string_representation(
    MobileSpacePoint const& point)
{
    return fmt::format(
            "MobileSpacePoint(pathname='{}')",
            point.id().pathname()
        );
}


static std::string informal_string_representation(
    MobileSpacePoint const& point)
{
    return fmt::format(
            "{}\n"
            "    nr_points: {}",
            formal_string_representation(point),
            point.nr_points()
        );
}

}  // Anonymous namespace

void init_mobile_space_point(
    py::module& module)
{

    py::class_<MobileSpacePoint, same_shape::constant_shape::Value>(
        module,
        "MobileSpacePoint",
        R"(
    TODO
)")

        .def(
            "__repr__",
            [](MobileSpacePoint const& point) {
                return formal_string_representation(point);
            }
        )

        .def(
            "__str__",
            [](MobileSpacePoint const& point) {
                return informal_string_representation(point);
            }
        )

        ;

}

}  // namespace lue
