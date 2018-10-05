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
        R"(
    Time discretization can be regular grid
)")
        .value("regular_grid", TimeDiscretization::regular_grid)
        ;

    py::enum_<SpaceDiscretization>(
        module,
        "SpaceDiscretization",
        R"(
    Space discretization can be regular grid
)")
        .value("regular_grid", SpaceDiscretization::regular_grid)
        ;

    py::class_<PropertyGroup, hdf5::Group>(
        module,
        "PropertyGroup")

        .def(
            "set_time_discretization",
            &PropertyGroup::set_time_discretization,
            "type"_a,
            "property"_a,
            R"(
    Set the time discretization

    :param TimeDiscretization type: Discretization type
    :param PropertyGroup property: Discretization property
)")

        .def(
            "set_space_discretization",
            &PropertyGroup::set_space_discretization,
            "type"_a,
            "property"_a,
            R"(
    Set the space discretization

    :param SpaceDiscretization type: Discretization type
    :param PropertyGroup property: Discretization property
)")

        ;

}

}  // namespace lue
