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
        R"(
    The shape of object arrays of different objects can be the same
    or different
)")
        .value("different", ShapePerObject::different)
        .value("same", ShapePerObject::same)
        ;


    py::enum_<ShapeVariability>(
        module,
        "ShapeVariability",
        R"(
    The shape of object arrays can be constant through time or variable
)")
        .value("constant", ShapeVariability::constant)
        .value("variable", ShapeVariability::variable)
        ;


    py::enum_<ValueVariability>(
        module,
        "ValueVariability",
        R"(
    The object array's elements can have a constant value through time
    or variable
)")
        .value("constant", ValueVariability::constant)
        .value("variable", ValueVariability::variable)
        ;


    BASE_COLLECTION(PropertySets, PropertySet)


    py::class_<PropertySets, PropertySetCollection>(
        module,
        "PropertySets",
        R"(
    Collection of property-sets

    Property-set collections can be obtained from :class:`Phenomenon`
    instances.
)")

        .def(
            "add",
            py::overload_cast<std::string const&>(&PropertySets::add),
            "name"_a,
            R"(
    Add new property-set to collection

    :param str name: Name of property-set to create
    :return: Property-set created
    :rtype: lue.PropertySet
    :raises RuntimeError: In case the property-set cannot be created

    The property-set will have no time domain and no space
    domain. Information stored in this property-set will be omnipresent
    through time and space.
)",
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
            "name"_a,
            "space_configuration"_a,
            "space_coordinate_dtype"_a,
            "rank"_a,
            R"(
    Add new property-set to collection

    :param str name: Name of property-set to create
    :param SpaceConfiguration space_configuration: Configuration of
        space domain
    :param numpy.dtype space_coordinate_dtype: Datatype of the spatial
        coordinates
    :param int rank: Number of spatial dimensions
    :return: Property-set created
    :rtype: lue.PropertySet
    :raises RuntimeError: In case the property-set cannot be created

    The property-set returned will have no time domain. Information
    stored in this property-set will be omnipresent through time.
)",
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
            "name"_a,
            "time_configuration"_a,
            "clock"_a,
            "space_configuration"_a,
            "space_coordinate_dtype"_a,
            "rank"_a,
            R"(
    Add new property-set to collection

    :param str name: Name of property-set to create
    :param TimeConfiguration time_configuration: Configuration of
        time domain
    :param lue.Clock clock: Clock for locations in time
    :param SpaceConfiguration space_configuration: Configuration of
        space domain
    :param numpy.dtype space_coordinate_dtype: Datatype of the spatial
        coordinates
    :param int rank: Number of spatial dimensions
    :return: Property-set created
    :rtype: lue.PropertySet
    :raises RuntimeError: In case the property-set cannot be created
)",
            py::return_value_policy::reference_internal)

        .def(
            "add",
            py::overload_cast<std::string const&, TimeDomain&>(
                &PropertySets::add),
            "name"_a,
            "time_domain"_a,
            R"(
    Add new property-set to collection

    :param str name: Name of property-set to create
    :param TimeDomain time_domain: Another property-set's ttime domain
        to use. Sharing time domains makes sense when the locations in
        time are the same. This saves space in the dataset.
    :return: Property-set created
    :rtype: lue.PropertySet
    :raises RuntimeError: In case the property-set cannot be created

    The property-set will have no space domain. Information stored in
    this property-set will be omnipresent through space.
)",
            py::return_value_policy::reference_internal)

        ;


    py::class_<PropertySet, hdf5::Group>(
        module,
        "PropertySet",
        R"(
    Class aggregating a time domain, space domain, and properties of a
    set of objects

    The information stored in a property-set is a consistant whole. In
    case a time domain is present, and a space domain is present and
    one or more properties are present, then for each location in time
    and for each active object, a location in space is stored, and
    for each property a value is stored.

    Which objects are active at each location in time is tracked by the
    :attr:`object tracker`.
)")

        .def_property_readonly("object_tracker",
            py::overload_cast<>(&PropertySet::object_tracker),
            R"(
    Return the active objects tracker

    :rtype: lue.ObjectTracker
)",
            py::return_value_policy::reference_internal)

        .def_property_readonly("has_time_domain",
            &PropertySet::has_time_domain,
            R"(
    Return whether the property-set has a time domain
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
    Return the time domain

    :rtype: lue.TimeDomain
    :raises RuntimeError: In case the property-set does not have a
        time domain
)",
            py::return_value_policy::reference_internal)

        .def_property_readonly("has_space_domain",
            &PropertySet::has_space_domain,
            R"(
    Return whether the property-set has a space domain
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
            R"(
    Return the space domain

    :rtype: lue.SpaceDomain
    :raises RuntimeError: In case the property-set does not have a
        space domain
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
            "name"_a,
            "dtype"_a,
            R"(
    Add new property to collection

    :param str name: Name of property to create
    :param dtype numpy.dtype: Datatype of object array elements
    :return: Property created
    :rtype: lue.same_shape.Property
    :raises RuntimeError: In case the property cannot be created
)",
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
            "name"_a,
            "dtype"_a,
            "shape"_a,
            R"(
    Add new property to collection

    :param str name: Name of property to create
    :param dtype numpy.dtype: Datatype of object array elements
    :param tuple shape: Shape of object arrays
    :return: Property created
    :rtype: lue.same_shape.Property
    :raises RuntimeError: In case the property cannot be created
)",
            py::return_value_policy::reference_internal)

        .def(
            "add_property",
            [](
                PropertySet& property_set,
                std::string const& name,
                py::dtype const& dtype,
                py::tuple const& shape,
                ValueVariability const value_variability)
            {
                // In this overload:
                // - Shape is the same per object
                // - Shape is constant through time
                // - Value is constant or variable
                auto const datatype_ = numpy_type_to_memory_datatype(dtype);
                auto const shape_ = tuple_to_shape(shape);

                py::object property = py::none();

                switch(value_variability) {
                    case ValueVariability::constant: {
                        using Property = same_shape::Property;
                        property = py::cast(
                            &property_set.properties().add<Property>(
                                name,
                                datatype_,
                                shape_));
                        break;
                    }
                    case ValueVariability::variable: {
                        using Property = same_shape::constant_shape::Property;
                        property = py::cast(
                            &property_set.properties().add<Property>(
                                name,
                                datatype_,
                                shape_));
                        break;
                    }
                }

                return property;
            },
            "name"_a,
            "dtype"_a,
            "shape"_a,
            "value_variability"_a,
            R"(
    Add new property to collection

    :param str name: Name of property to create
    :param dtype numpy.dtype: Datatype of object array elements
    :param tuple shape: Shape of object arrays
    :param ValueVariability value_variability: Value variability
    :return: Property created
    :rtype: lue.same_shape.Property or
        lue.same_shape.constant_shape.Property, depending on the
        *value_variability* passed in
    :raises RuntimeError: In case the property cannot be created
)",
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
            "name"_a,
            "dtype"_a,
            "rank"_a,
            R"(
    Add new property to collection

    :param str name: Name of property to create
    :param dtype numpy.dtype: Datatype of object array elements
    :param int rank: Rank of object arrays
    :return: Property created
    :rtype: lue.different_shape.Property
    :raises RuntimeError: In case the property cannot be created
)",
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
            "name"_a,
            "dtype"_a,
            "rank"_a,
            "shape_per_object"_a,
            "shape_variability"_a,
            R"(
    Add new property to collection

    :param str name: Name of property to create
    :param dtype numpy.dtype: Datatype of object array elements
    :param int rank: Rank of object arrays
    :param ShapePerObject shape_per_object: Shape per object
    :param ShapeVariability shape_variability: Shape variability
    :return: Property created
    :rtype: lue.same_shape::variable_shape::Property,
        lue.different_shape::constant_shape::Property or
        lue.different_shape::variable_shape::Property depending on
        *shape_per_object* and *shape_variability* passed in
    :raises RuntimeError: In case the property cannot be created
)",
            py::return_value_policy::reference_internal)

        ;

}

}  // namespace lue
