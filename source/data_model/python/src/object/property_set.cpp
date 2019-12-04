#include "../python_extension.hpp"
#include "lue/object/property/property_set.hpp"
#include "lue/object/property_sets.hpp"
#include "../core/collection.hpp"
#include "lue/py/conversion.hpp"
#include <boost/algorithm/string/join.hpp>
#include <algorithm>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace {

static std::string formal_string_representation(
    PropertySet const& property_set)
{
    return fmt::format(
        "PropertySet(pathname='{}')",
        property_set.id().pathname());
}


template<
    typename PropertiesSpecialization>
std::vector<std::string> property_names(
    Properties const& properties)
{
    return properties.collection<PropertiesSpecialization>().names();
}


template<
    typename PropertiesSpecialization>
std::string join_property_names(
    Properties const& properties)
{
    return boost::algorithm::join(
        property_names<PropertiesSpecialization>(properties),
        ", ");
}


std::vector<std::string>& join_collections(
    std::vector<std::string> const& from_collection,
    std::vector<std::string>& to_collection)
{
    std::copy(
        from_collection.begin(), from_collection.end(),
        std::back_inserter(to_collection));

    return to_collection;
}


static std::string informal_string_representation(
    PropertySet const& property_set)
{
    auto const& properties = property_set.properties();

    return fmt::format(
        "{}\n"
        "    same_shape properties: [{}]\n"
        "    same_shape/constant_shape properties: [{}]\n"
        "    same_shape/variable_shape properties: [{}]\n"
        "    different_shape properties: [{}]\n"
        "    different_shape/constant_shape properties: [{}]\n"
        "    different_shape/variable_shape properties: [{}]",
        formal_string_representation(property_set),
        join_property_names<lue::same_shape::Properties>(properties),
        join_property_names<lue::same_shape::constant_shape::Properties>(properties),
        join_property_names<lue::same_shape::variable_shape::Properties>(properties),
        join_property_names<lue::different_shape::Properties>(properties),
        join_property_names<lue::different_shape::constant_shape::Properties>(properties),
        join_property_names<lue::different_shape::variable_shape::Properties>(properties)
        );
}

}  // Anonymous namespace


// Return a py::object to a property of the right type. Note that a
// new instance of the property is stored.
// TODO Refactor this with property_reference
py::object property_new(
    Properties& properties,
    std::string const& name)
{
    py::object property = py::none();
    auto const shape_per_object =
        properties.shape_per_object(name);
    auto const value_variability =
        properties.value_variability(name);

    switch(value_variability) {
        case ValueVariability::constant: {
            switch(shape_per_object) {
                case ShapePerObject::same: {
                    using Collection = same_shape::Properties;
                    using Property = same_shape::Property;
                    property = py::cast(new
                        Property{
                            properties.collection<Collection>()[name].parent(),
                            name
                        }
                    );
                    break;
                }
                case ShapePerObject::different: {
                    using Collection = different_shape::Properties;
                    using Property = different_shape::Property;
                    property = py::cast(new
                        Property{
                            properties.collection<Collection>()[name].parent(),
                            name
                        }
                    );
                    break;
                }
            }

            break;
        }
        case ValueVariability::variable: {
            auto const shape_variability =
                properties.shape_variability(name);

            switch(shape_per_object) {
                case ShapePerObject::same: {

                    switch(shape_variability) {
                        case ShapeVariability::constant: {
                            using Collection =
                                same_shape::constant_shape::Properties;
                            using Property = same_shape::constant_shape::Property;
                            property = py::cast(new
                                Property{
                                    properties.collection<Collection>()[name].parent(),
                                    name
                                }
                            );
                            break;
                        }
                        case ShapeVariability::variable: {
                            using Collection = same_shape::variable_shape::Properties;
                            using Property = same_shape::variable_shape::Property;
                            property = py::cast(new
                                Property{
                                    properties.collection<Collection>()[name].parent(),
                                    name
                                }
                            );
                            break;
                        }
                    }

                    break;
                }
                case ShapePerObject::different: {

                    switch(shape_variability) {
                        case ShapeVariability::constant: {
                            using Collection =
                                different_shape::constant_shape::Properties;
                            using Property = different_shape::constant_shape::Property;
                            property = py::cast(new
                                Property{
                                    properties.collection<Collection>()[name].parent(),
                                    name
                                }
                            );
                            break;
                        }
                        case ShapeVariability::variable: {
                            using Collection =
                                different_shape::variable_shape::Properties;
                            using Property = different_shape::variable_shape::Property;
                            property = py::cast(new
                                Property{
                                    properties.collection<Collection>()[name].parent(),
                                    name
                                }
                            );
                            break;
                        }
                    }

                    break;
                }
            }

            break;
        }
    }

    return property;
}


// Return a py::object to a property of the right type. Note that a
// reference to the property is stored. The lifetime of this object
// is tied to the properties instance passed in.
// TODO Refactor this with property_new
py::object property_reference(
    Properties& properties,
    std::string const& name)
{
    py::object property = py::none();
    auto const shape_per_object =
        properties.shape_per_object(name);
    auto const value_variability =
        properties.value_variability(name);

    switch(value_variability) {
        case ValueVariability::constant: {
            switch(shape_per_object) {
                case ShapePerObject::same: {
                    using Collection = same_shape::Properties;
                    property = py::cast(&properties.collection<Collection>()[name]);
                    break;
                }
                case ShapePerObject::different: {
                    using Collection = different_shape::Properties;
                    property = py::cast(&properties.collection<Collection>()[name]);
                    break;
                }
            }

            break;
        }
        case ValueVariability::variable: {
            auto const shape_variability =
                properties.shape_variability(name);

            switch(shape_per_object) {
                case ShapePerObject::same: {

                    switch(shape_variability) {
                        case ShapeVariability::constant: {
                            using Collection =
                                same_shape::constant_shape::Properties;
                            property = py::cast(&properties.collection<Collection>()[name]);
                            break;
                        }
                        case ShapeVariability::variable: {
                            using Collection = same_shape::variable_shape::Properties;
                            property = py::cast(&properties.collection<Collection>()[name]);
                            break;
                        }
                    }

                    break;
                }
                case ShapePerObject::different: {

                    switch(shape_variability) {
                        case ShapeVariability::constant: {
                            using Collection =
                                different_shape::constant_shape::Properties;
                            property = py::cast(&properties.collection<Collection>()[name]);
                            break;
                        }
                        case ShapeVariability::variable: {
                            using Collection =
                                different_shape::variable_shape::Properties;
                            property = py::cast(&properties.collection<Collection>()[name]);

                            break;
                        }
                    }

                    break;
                }
            }

            break;
        }
    }

    return property;
}


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

        ;


    py::class_<Properties, hdf5::Group>(
        module,
        "Properties",
        R"(
    TODO docstring
)")

        .def(
            "__getitem__",
            [](
                Properties& properties,
                std::string const& name)
            {
                return property_reference(properties, name);
            },
            py::return_value_policy::reference_internal)

        .def_property_readonly(
            "names",
            [](
                Properties const& properties)
            {
                std::vector<std::string> names;

                join_collections(
                    property_names<lue::same_shape::Properties>(properties),
                    names);
                join_collections(
                    property_names<lue::same_shape::constant_shape::Properties>(properties),
                    names);
                join_collections(
                    property_names<lue::same_shape::variable_shape::Properties>(properties),
                    names);
                join_collections(
                    property_names<lue::different_shape::Properties>(properties),
                    names);
                join_collections(
                    property_names<lue::different_shape::constant_shape::Properties>(properties),
                    names);
                join_collections(
                    property_names<lue::different_shape::variable_shape::Properties>(properties),
                    names);

                return names;
            })

        .def(
            "value_variability",
            [](
                Properties const& properties,
                std::string const& name)
            {
                return properties.value_variability(name);
            })

        .def(
            "shape_per_object",
            [](
                Properties const& properties,
                std::string const& name)
            {
                return properties.shape_per_object(name);
            })

        .def(
            "shape_variability",
            [](
                Properties const& properties,
                std::string const& name)
            {
                return properties.shape_variability(name);
            })

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
    :attr:`object_tracker`.
)")

        .def(
            "__repr__",
            [](PropertySet const& property_set) {
                return formal_string_representation(property_set);
            }
        )

        .def(
            "__str__",
            [](PropertySet const& property_set) {
                return informal_string_representation(property_set);
            }
        )

        .def_property_readonly("object_tracker",
            py::overload_cast<>(&PropertySet::object_tracker),
            R"(
    Return the active objects tracker

    :rtype: ObjectTracker
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

    :rtype: TimeDomain
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

    :rtype: SpaceDomain
    :raises RuntimeError: In case the property-set does not have a
        space domain
)",
            py::return_value_policy::reference_internal)

        .def_property_readonly(
            "properties",
            py::overload_cast<>(&PropertySet::properties),
            R"(
    TODO docstring
)",
            py::return_value_policy::reference_internal)

        .def(
            "__getattr__",
            [](
                PropertySet& property_set,
                std::string const& property_name)
            {
                if(!property_set.properties().contains(property_name)) {
                    // TODO We are throwing a KeyError here. Should be
                    // an AttributeError, but pybind11 does not seem to
                    // support that yet.
                    //
                    // Python message:
                    // AttributeError: 'x' object has no attribute 'y'
                    // Ours is a little bit different:
                    throw pybind11::key_error(fmt::format(
                        "Property set does not contain property '{}'",
                        property_name));
                }

                return property_reference(
                    property_set.properties(), property_name);
            },
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
    :rtype: same_shape.Property
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
    :rtype: same_shape.Property
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
    :rtype: same_shape.Property or
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
    :rtype: different_shape.Property
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
    :rtype: same_shape::variable_shape::Property,
        different_shape::constant_shape::Property or
        different_shape::variable_shape::Property depending on
        *shape_per_object* and *shape_variability* passed in
    :raises RuntimeError: In case the property cannot be created
)",
            py::return_value_policy::reference_internal)

        ;

}

}  // namespace lue
