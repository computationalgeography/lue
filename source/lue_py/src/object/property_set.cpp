#include "lue/object/property/property_set.hpp"
#include "lue/object/property_sets.hpp"
#include "../core/collection.hpp"
#include "lue/py/conversion.hpp"


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {

void init_property_set(
    py::module& module)
{

    py::enum_<ShapePerObject>(
        module,
        "ShapePerObject",
        "ShapePerObject docstring...")
        .value("different", ShapePerObject::different)
        .value("same", ShapePerObject::same)
        ;

    py::enum_<ShapeVariability>(
        module,
        "ShapeVariability",
        "ShapeVariability docstring...")
        .value("constant", ShapeVariability::constant)
        .value("variable", ShapeVariability::variable)
        ;


    BASE_COLLECTION(PropertySets, PropertySet)

    py::class_<PropertySets, PropertySetCollection>(
        module,
        "PropertySets",
        R"(
    TODO
)")

        .def(
            "add",
            [](
                PropertySets& property_sets,
                std::string const& name) -> PropertySet&
            {
                return property_sets.add(name);
            },
            R"(
    Add new property set to collection
)",
            "name"_a,
            py::return_value_policy::reference_internal)

        .def(
            "add",
            [](
                PropertySets& property_sets,
                std::string const& name,
                SpaceConfiguration const& space_configuration,
                py::dtype const& space_coordinate_dtype,
                Rank const rank) -> PropertySet&
            {
                auto const datatype =
                    numpy_type_to_memory_datatype(space_coordinate_dtype);

                return property_sets.add(
                    name,
                    space_configuration, datatype, rank);
            },
            R"(
    Add new property set to collection
)",
            "name"_a,
            "space_configuration"_a,
            "space_coordinate_dtype"_a,
            "rank"_a,
            py::return_value_policy::reference_internal)

        .def(
            "add",
            [](
                PropertySets& property_sets,
                std::string const& name,
                TimeConfiguration const& time_configuration,
                Clock const& clock,
                SpaceConfiguration const& space_configuration,
                py::dtype const& space_coordinate_dtype,
                Rank const rank) -> PropertySet&
            {
                auto const datatype =
                    numpy_type_to_memory_datatype(space_coordinate_dtype);

                return property_sets.add(
                    name,
                    time_configuration, clock,
                    space_configuration, datatype, rank);
            },
            R"(
    Add new property set to collection
)",
            "name"_a,
            "time_configuration"_a,
            "clock"_a,
            "space_configuration"_a,
            "space_coordinate_dtype"_a,
            "rank"_a,
            py::return_value_policy::reference_internal)

        .def(
            "add",
            [](
                PropertySets& property_sets,
                std::string const& name,
                TimeDomain& domain) -> PropertySet&
            {
                return property_sets.add(name, domain);
            },
            R"(
    Add new property set to collection
)",
            "name"_a,
            "time_domain"_a,
            py::return_value_policy::reference_internal)

        ;


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

        .def_property_readonly("has_time_domain",
            &PropertySet::has_time_domain,
            R"(
    TODO
)")

        .def_property_readonly("time_domain",
            [](
                PropertySet& property_set) -> TimeDomain&
            {
                if(!property_set.has_time_domain()) {
                    throw std::runtime_error(
                        "Property set does not have a time domain");
                }

                return property_set.time_domain();
            },
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
            [](
                PropertySet& property_set) -> SpaceDomain&
            {
                if(!property_set.has_space_domain()) {
                    throw std::runtime_error(
                        "Property set does not have a space domain");
                }

                return property_set.space_domain();
            },
            // py::overload_cast<>(&PropertySet::space_domain),
            R"(
    TODO
)",
            py::return_value_policy::reference_internal)

        .def(
            "add_property",
            [](
                PropertySet& property_set,
                std::string const& name,
                py::dtype const& dtype) -> same_shape::Property&
            {
                auto const datatype_ = numpy_type_to_memory_datatype(dtype);

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
                py::dtype const& dtype,
                py::tuple const& shape) -> same_shape::Property&
            {
                auto const datatype_ = numpy_type_to_memory_datatype(dtype);
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
                py::dtype const& dtype,
                Rank const rank) -> different_shape::Property&
            {
                // In this overload
                // - Value is constant
                // - Shape differs per object
                auto const datatype_ = numpy_type_to_memory_datatype(dtype);

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

        .def(
            "add_property",
            [](
                PropertySet& property_set,
                std::string const& name,
                py::dtype const& dtype,
                Rank const rank,
                ShapePerObject const shape_per_object,
                ShapeVariability const shape_variability)
            {
                // In this overload:
                // - Value is variable, and
                // - Shape differs per object, and/or
                // - Shape differs through time
                auto const datatype_ = numpy_type_to_memory_datatype(dtype);

                py::object property = py::none();

                switch(shape_per_object) {
                    case ShapePerObject::same: {
                        switch(shape_variability) {
                            case ShapeVariability::constant: {
                                throw std::runtime_error(
                                    "Pass in shape of object array if "
                                    "these don't differ per object and "
                                    "are constant through time");
                                break;
                            }
                            case ShapeVariability::variable: {
                                using Property = same_shape::variable_shape::Property;
                                property = py::cast(
                                    &property_set.properties().add<Property>(
                                        name,
                                        datatype_,
                                        rank));
                                break;
                            }
                        }

                        break;
                    }
                    case ShapePerObject::different: {
                        switch(shape_variability) {
                            case ShapeVariability::constant: {
                                using Property = different_shape::constant_shape::Property;
                                property = py::cast(
                                    &property_set.properties().add<Property>(
                                        name,
                                        datatype_,
                                        rank));
                                break;
                            }
                            case ShapeVariability::variable: {
                                using Property = different_shape::variable_shape::Property;
                                property = py::cast(
                                    &property_set.properties().add<Property>(
                                        name,
                                        datatype_,
                                        rank));
                                break;
                            }
                        }

                        break;
                    }
                }

                return property;
            },
            R"(
    Add new property to collection
)",
            "name"_a,
            "dtype"_a,
            "rank"_a,
            "shape_per_object"_a,
            "shape_variability"_a,
            py::return_value_policy::reference_internal)

        ;

}

}  // namespace lue
