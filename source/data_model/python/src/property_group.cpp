#include "python_extension.hpp"
#include "lue/info/property/property_group.hpp"
#include "lue/navigate.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace data_model {

// Defined in property_set.cpp...
// TODO Reorganize this in a header / module
py::object         property_new        (Properties& properties,
                                        std::string const& name);


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

        .def_property_readonly(
            "time_is_discretized",
            &PropertyGroup::time_is_discretized)

        .def_property_readonly(
            "space_is_discretized",
            &PropertyGroup::space_is_discretized)

        .def(
            "space_discretization_property",
            [](
                PropertyGroup& property)
            {
                assert(property.space_is_discretized());

                auto discretization_property =
                    property.space_discretization_property();

                // Property-set the discretization property is member of.
                // New, local instance.
                auto property_set =
                    PropertySet{data_model::property_set(discretization_property)};

                assert(property_set.properties().contains(
                        discretization_property.name()));

                // Return a new instance of a property of the right type
                return data_model::property_new(
                    property_set.properties(), discretization_property.name());
            })
        ;

}

}  // namespace data_model
}  // namespace lue
