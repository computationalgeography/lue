#include "lue/object/property/property_set.hpp"
// #include "../core/collection.hpp"
#include "lue/py/conversion.hpp"
#include <pybind11/pybind11.h>
#include <iostream>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {

void init_property_set(
    py::module& module)
{

    py::class_<PropertySet, hdf5::Group>(
        module,
        "PropertySet",
        R"(
    TODO
)")

        .def_property_readonly("object_tracker",
            py::overload_cast<>(&PropertySet::object_tracker),
            R"(
    TODO
)",
            py::return_value_policy::reference_internal)

        .def_property_readonly("has_space_domain",
            &PropertySet::has_space_domain,
            R"(
    TODO
)")

        .def_property_readonly("space_domain",
            py::overload_cast<>(&PropertySet::space_domain),
            R"(
    TODO
)",
            py::return_value_policy::reference_internal)

        .def(
            "add_property",
            [](
                PropertySet& property_set,
                std::string const& name,
                py::handle const& datatype) -> same_shape::Property&
            {
                auto const datatype_ = numpy_type_to_memory_datatype(datatype);

                return property_set.properties().add(
                    name,
                    datatype_);
            },
            R"(
    Add new property to collection
)",
            "name"_a,
            "dtype"_a,
            py::return_value_policy::reference_internal)

        .def(
            "add_property",
            [](
                PropertySet& property_set,
                std::string const& name,
                py::handle const& datatype,
                py::tuple const& shape) -> same_shape::Property&
            {
                auto const datatype_ = numpy_type_to_memory_datatype(datatype);
                auto const shape_ = tuple_to_shape(shape);

                return property_set.properties().add(
                    name,
                    datatype_,
                    shape_);
            },
            R"(
    Add new property to collection
)",
            "name"_a,
            "dtype"_a,
            "shape"_a,
            py::return_value_policy::reference_internal)

        .def(
            "add_property",
            [](
                PropertySet& property_set,
                std::string const& name,
                py::handle const& datatype,
                Rank const rank) -> different_shape::Property&
            {
                auto const datatype_ = numpy_type_to_memory_datatype(datatype);

                return property_set.properties().add(
                    name,
                    datatype_,
                    rank);
            },
            R"(
    Add new property to collection
)",
            "name"_a,
            "dtype"_a,
            "rank"_a,
            py::return_value_policy::reference_internal)

        ;

}

}  // namespace lue
