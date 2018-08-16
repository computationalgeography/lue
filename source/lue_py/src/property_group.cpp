#include "lue/info/property/property_group.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {

void init_property_group(
    py::module& module)
{

    py::enum_<TimeDiscretization>(
        module,
        "TimeDiscretization",
        "TimeDiscretization docstring...")
        .value("regular_grid", TimeDiscretization::regular_grid)
        ;

    py::enum_<SpaceDiscretization>(
        module,
        "SpaceDiscretization",
        "SpaceDiscretization docstring...")
        .value("regular_grid", SpaceDiscretization::regular_grid)
        ;

    py::class_<PropertyGroup, hdf5::Group>(
        module,
        "PropertyGroup",
        R"(
    PropertyGroup docstring...
)")

        .def(
            "set_time_discretization",
            &PropertyGroup::set_time_discretization)

        .def(
            "set_space_discretization",
            &PropertyGroup::set_space_discretization)

        ;

}

}  // namespace lue
