#include "../core/collection.hpp"
#include "../python_extension.hpp"
#include "lue/object/phenomena.hpp"
#include "lue/py/data_model/conversion.hpp"
#include <boost/algorithm/string/join.hpp>
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
    namespace data_model {
        namespace {

            static std::string formal_string_representation(Phenomenon const& phenomenon)
            {
                return std::format("Phenomenon(pathname='{}')", phenomenon.id().pathname());
            }


            static std::string informal_string_representation(Phenomenon const& phenomenon)
            {
                return std::format(
                    "{}\n"
                    "    collection-property-sets: [{}]\n"
                    "    property-sets: [{}]",
                    formal_string_representation(phenomenon),
                    boost::algorithm::join(phenomenon.collection_property_sets().names(), ", "),
                    boost::algorithm::join(phenomenon.property_sets().names(), ", "));
            }


            PropertySet& add_property_set(PropertySets& property_sets, std::string const& name)
            {
                return property_sets.add(name);
            }


            PropertySet& add_property_set(
                PropertySets& property_sets,
                std::string const& name,
                SpaceConfiguration const& space_configuration,
                py::dtype const& space_coordinate_dtype,
                Rank const rank)
            {
                auto const datatype = numpy_type_to_memory_datatype(space_coordinate_dtype);

                return property_sets.add(name, space_configuration, datatype, rank);
            }


            PropertySet& add_property_set(
                PropertySets& property_sets,
                std::string const& name,
                TimeConfiguration const& time_configuration,
                Clock const& clock)
            {
                return property_sets.add(name, time_configuration, clock);
            }


            PropertySet& add_property_set(
                PropertySets& property_sets,
                std::string const& name,
                TimeConfiguration const& time_configuration,
                Clock const& clock,
                SpaceConfiguration const& space_configuration,
                py::dtype const& space_coordinate_dtype,
                Rank const rank)
            {
                auto const datatype = numpy_type_to_memory_datatype(space_coordinate_dtype);

                return property_sets.add(
                    name, time_configuration, clock, space_configuration, datatype, rank);
            }


            PropertySet& add_property_set(
                PropertySets& property_sets, std::string const& name, TimeDomain& time_domain)
            {
                return property_sets.add(name, time_domain);
            }


            PropertySet& add_property_set(
                PropertySets& property_sets,
                std::string const& name,
                TimeConfiguration const& time_configuration,
                Clock const& clock,
                ObjectTracker& object_tracker)
            {
                return property_sets.add(name, time_configuration, clock, object_tracker);
            }


            PropertySet& add_property_set(
                PropertySets& property_sets,
                std::string const& name,
                TimeDomain& time_domain,
                ObjectTracker& object_tracker)
            {
                return property_sets.add(name, time_domain, object_tracker);
            }


            PropertySet& add_property_set(
                PropertySets& property_sets,
                std::string const& name,
                TimeDomain& time_domain,
                SpaceConfiguration const& space_configuration,
                py::dtype const& space_coordinate_dtype,
                Rank const rank)
            {
                auto const datatype = numpy_type_to_memory_datatype(space_coordinate_dtype);

                return property_sets.add(name, time_domain, space_configuration, datatype, rank);
            }


            PropertySet& add_property_set(
                PropertySets& property_sets,
                std::string const& name,
                TimeDomain& time_domain,
                ObjectTracker& object_tracker,
                SpaceConfiguration const& space_configuration,
                py::dtype const& space_coordinate_dtype,
                Rank const rank)
            {
                auto const datatype = numpy_type_to_memory_datatype(space_coordinate_dtype);

                return property_sets.add(
                    name, time_domain, object_tracker, space_configuration, datatype, rank);
            }

        }  // Anonymous namespace


        void init_phenomenon(py::module& module)
        {

            BASE_COLLECTION(Phenomena, Phenomenon)


            py::class_<Phenomena, PhenomenonCollection>(
                module,
                "Phenomena",
                R"(
    Collection of phenomena

    Phenomenon collections can be obtained from :class:`Dataset` and
    :class:`Universe` instances.
)")

                ;


            py::class_<Phenomenon, hdf5::Group>(
                module,
                "Phenomenon",
                R"(
    Class for representing information about a collection of objects

    A phenomenon contains two :class:`property set collections
    <PropertySets>`. The :meth:`first <Phenomenon.property_sets>`
    contains information about each object in a collection of objects. The
    :meth:`second <Phenomenon.collection_property_sets>` second contains
    information that is representable for the collection of objects as
    a whole.

    Phenomena are used to organize information about different collections
    of objects. For example, information about cows, parcels and farmers
    can be stored using three phenomena.
)")

                .def(
                    "__repr__",
                    [](Phenomenon const& phenomenon) { return formal_string_representation(phenomenon); })

                .def(
                    "__str__",
                    [](Phenomenon const& phenomenon) { return informal_string_representation(phenomenon); })

                .def_property_readonly(
                    "description",
                    &Phenomenon::description,
                    R"(
    Return description
)")

                .def_property_readonly(
                    "object_id",
                    py::overload_cast<>(&Phenomenon::object_id),
                    R"(
    object_id docstring...
)")

                .def_property_readonly(
                    "property_sets",
                    py::overload_cast<>(&Phenomenon::property_sets),
                    R"(
    Return property sets collection containing information about each
    object in the collection of objects

    :rtype: PropertySets
)",
                    py::return_value_policy::reference_internal)

                .def_property_readonly(
                    "collection_property_sets",
                    py::overload_cast<>(&Phenomenon::collection_property_sets),
                    R"(
    Return property sets collection containing information about the
    collection of objects

    :rtype: PropertySets
)",
                    py::return_value_policy::reference_internal)

                .def(
                    "__getattr__",
                    [](Phenomenon& phenomenon, std::string const& property_set_name)
                    {
                        auto const property_set_found =
                            phenomenon.property_sets().contains(property_set_name);
                        auto const collection_property_set_found =
                            phenomenon.collection_property_sets().contains(property_set_name);


                        if (!property_set_found && !collection_property_set_found)
                        {
                            // TODO We are throwing a KeyError here. Should be
                            // an AttributeError, but pybind11 does not seem to
                            // support that yet.
                            //
                            // Python message:
                            // AttributeError: 'x' object has no attribute 'y'
                            // Ours is a little bit different:
                            throw pybind11::key_error(std::format(
                                "Phenomenon does not contain (collection) "
                                "property-set '{}'",
                                property_set_name));
                        }

                        assert(property_set_found || collection_property_set_found);

                        auto& property_sets = property_set_found ? phenomenon.property_sets()
                                                                 : phenomenon.collection_property_sets();

                        return py::cast(&property_sets[property_set_name]);
                    },
                    py::return_value_policy::reference_internal)

                .def(
                    "add_property_set",
                    [](Phenomenon& phenomenon, std::string const& name) -> PropertySet&
                    { return add_property_set(phenomenon.property_sets(), name); },
                    "name"_a,
                    R"(
    Add new property-set to collection

    :param str name: Name of property-set to create
    :return: Property-set created
    :rtype: PropertySet
    :raises RuntimeError: In case the property-set cannot be created

    The property-set will have no time domain and no space
    domain. Information stored in this property-set will be omnipresent
    through time and space.
)",
                    py::return_value_policy::reference_internal)

                .def(
                    "add_collection_property_set",
                    [](Phenomenon& phenomenon, std::string const& name) -> PropertySet&
                    { return add_property_set(phenomenon.collection_property_sets(), name); },
                    "name"_a,
                    R"(
    Add new property-set to collection

    :param str name: Name of property-set to create
    :return: Property-set created
    :rtype: PropertySet
    :raises RuntimeError: In case the property-set cannot be created

    The property-set will have no time domain and no space
    domain. Information stored in this property-set will be omnipresent
    through time and space.
)",
                    py::return_value_policy::reference_internal)

                .def(
                    "add_property_set",
                    [](Phenomenon& phenomenon,
                       std::string const& name,
                       SpaceConfiguration const& space_configuration,
                       py::object const& space_coordinate_dtype_args,
                       Rank const rank) -> PropertySet&
                    {
                        return add_property_set(
                            phenomenon.property_sets(),
                            name,
                            space_configuration,
                            py::dtype::from_args(space_coordinate_dtype_args),
                            rank);
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
    :rtype: PropertySet
    :raises RuntimeError: In case the property-set cannot be created

    The property-set returned will have no time domain. Information
    stored in this property-set will be omnipresent through time.
)",
                    py::return_value_policy::reference_internal)

                .def(
                    "add_property_set",
                    [](Phenomenon& phenomenon,
                       std::string const& name,
                       TimeConfiguration const& time_configuration,
                       Clock const& clock) -> PropertySet&
                    { return add_property_set(phenomenon.property_sets(), name, time_configuration, clock); },
                    "name"_a,
                    "time_configuration"_a,
                    "clock"_a,
                    R"(
    Add new property-set to collection

    :param str name: Name of property-set to create
    :param TimeConfiguration time_configuration: Configuration of
        time domain
    :param Clock clock: Clock for locations in time
    :return: Property-set created
    :rtype: PropertySet
    :raises RuntimeError: In case the property-set cannot be created
)",
                    py::return_value_policy::reference_internal)

                .def(
                    "add_property_set",
                    [](Phenomenon& phenomenon,
                       std::string const& name,
                       TimeConfiguration const& time_configuration,
                       Clock const& clock,
                       SpaceConfiguration const& space_configuration,
                       py::object const& space_coordinate_dtype_args,
                       Rank const rank) -> PropertySet&
                    {
                        return add_property_set(
                            phenomenon.property_sets(),
                            name,
                            time_configuration,
                            clock,
                            space_configuration,
                            py::dtype::from_args(space_coordinate_dtype_args),
                            rank);
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
    :param Clock clock: Clock for locations in time
    :param SpaceConfiguration space_configuration: Configuration of
        space domain
    :param numpy.dtype space_coordinate_dtype: Datatype of the spatial
        coordinates
    :param int rank: Number of spatial dimensions
    :return: Property-set created
    :rtype: PropertySet
    :raises RuntimeError: In case the property-set cannot be created
)",
                    py::return_value_policy::reference_internal)

                .def(
                    "add_property_set",
                    [](Phenomenon& phenomenon,
                       std::string const& name,
                       TimeDomain& time_domain) -> PropertySet&
                    { return add_property_set(phenomenon.property_sets(), name, time_domain); },
                    "name"_a,
                    "time_domain"_a,
                    R"(
    Add new property-set to collection

    :param str name: Name of property-set to create
    :param TimeDomain time_domain: Another property-set's time domain
        to use. Sharing time domains makes sense when the locations in
        time are the same. This saves space in the dataset.
    :return: Property-set created
    :rtype: PropertySet
    :raises RuntimeError: In case the property-set cannot be created

    The property-set will have no space domain. Information stored in
    this property-set will be omnipresent through space.
)",
                    py::return_value_policy::reference_internal)

                .def(
                    "add_collection_property_set",
                    [](Phenomenon& phenomenon,
                       std::string const& name,
                       TimeDomain& time_domain) -> PropertySet&
                    { return add_property_set(phenomenon.collection_property_sets(), name, time_domain); },
                    "name"_a,
                    "time_domain"_a,
                    R"(
    Add new property-set to collection

    :param str name: Name of property-set to create
    :param TimeDomain time_domain: Another property-set's time domain
        to use. Sharing time domains makes sense when the locations in
        time are the same. This saves space in the dataset.
    :return: Property-set created
    :rtype: PropertySet
    :raises RuntimeError: In case the property-set cannot be created

    The property-set will have no space domain. Information stored in
    this property-set will be omnipresent through space.
)",
                    py::return_value_policy::reference_internal)

                .def(
                    "add_property_set",
                    [](Phenomenon& phenomenon,
                       std::string const& name,
                       TimeConfiguration const& time_configuration,
                       Clock const& clock,
                       ObjectTracker& object_tracker) -> PropertySet& {
                        return add_property_set(
                            phenomenon.property_sets(), name, time_configuration, clock, object_tracker);
                    },
                    "name"_a,
                    "time_configuration"_a,
                    "clock"_a,
                    "object_tracker"_a,
                    R"(
    Add new property-set to collection

    :param str name: Name of property-set to create
    :param TimeConfiguration time_configuration: Configuration of
        time domain
    :param Clock clock: Clock for locations in time
    :param ObjectTracker object_tracker: Another property-set's object
        tracker to use. Sharing object trackers makes sense when the
        active set in different property-sets are the same.
    :return: Property-set created
    :rtype: PropertySet
    :raises RuntimeError: In case the property-set cannot be created

    The property-set will have no space domain. Information stored in
    this property-set will be omnipresent through space.
)",
                    py::return_value_policy::reference_internal)

                .def(
                    "add_property_set",
                    [](Phenomenon& phenomenon,
                       std::string const& name,
                       TimeDomain& time_domain,
                       ObjectTracker& object_tracker) -> PropertySet& {
                        return add_property_set(
                            phenomenon.property_sets(), name, time_domain, object_tracker);
                    },
                    "name"_a,
                    "time_domain"_a,
                    "object_tracker"_a,
                    R"(
    Add new property-set to collection

    :param str name: Name of property-set to create
    :param TimeDomain time_domain: Another property-set's time domain
        to use. Sharing time domains makes sense when the locations in
        time are the same. This saves space in the dataset.
    :param ObjectTracker object_tracker: Another property-set's object
        tracker to use. Sharing object trackers makes sense when the
        active set in different property-sets are the same.
    :return: Property-set created
    :rtype: PropertySet
    :raises RuntimeError: In case the property-set cannot be created

    The property-set will have no space domain. Information stored in
    this property-set will be omnipresent through space.
)",
                    py::return_value_policy::reference_internal)

                .def(
                    "add_property_set",
                    [](Phenomenon& phenomenon,
                       std::string const& name,
                       TimeDomain& time_domain,
                       SpaceConfiguration const& space_configuration,
                       py::object const& space_coordinate_dtype_args,
                       Rank const rank) -> PropertySet&
                    {
                        return add_property_set(
                            phenomenon.property_sets(),
                            name,
                            time_domain,
                            space_configuration,
                            py::dtype::from_args(space_coordinate_dtype_args),
                            rank);
                    },
                    "name"_a,
                    "time_domain"_a,
                    "space_configuration"_a,
                    "space_coordinate_dtype"_a,
                    "rank"_a,
                    R"(
    Add new property-set to collection

    :param str name: Name of property-set to create
    :param TimeDomain time_domain: Another property-set's time domain
        to use. Sharing time domains makes sense when the locations in
        time are the same. This saves space in the dataset.
    :param SpaceConfiguration space_configuration: Configuration of
        space domain
    :param numpy.dtype space_coordinate_dtype: Datatype of the spatial
        coordinates
    :param int rank: Number of spatial dimensions
    :return: Property-set created
    :rtype: PropertySet
    :raises RuntimeError: In case the property-set cannot be created
)",
                    py::return_value_policy::reference_internal)

                .def(
                    "add_property_set",
                    [](Phenomenon& phenomenon,
                       std::string const& name,
                       TimeDomain& time_domain,
                       ObjectTracker& object_tracker,
                       SpaceConfiguration const& space_configuration,
                       py::object const& space_coordinate_dtype_args,
                       Rank const rank) -> PropertySet&
                    {
                        return add_property_set(
                            phenomenon.property_sets(),
                            name,
                            time_domain,
                            object_tracker,
                            space_configuration,
                            py::dtype::from_args(space_coordinate_dtype_args),
                            rank);
                    },
                    "name"_a,
                    "time_domain"_a,
                    "object_tracker"_a,
                    "space_configuration"_a,
                    "space_coordinate_dtype"_a,
                    "rank"_a,
                    R"(
    Add new property-set to collection

    :param str name: Name of property-set to create
    :param TimeDomain time_domain: Another property-set's time domain
        to use. Sharing time domains makes sense when the locations in
        time are the same. This saves space in the dataset.
    :param ObjectTracker object_tracker: Another property-set's object
        tracker to use. Sharing object trackers makes sense when the
        active set in different property-sets are the same.
    :param SpaceConfiguration space_configuration: Configuration of
        space domain
    :param numpy.dtype space_coordinate_dtype: Datatype of the spatial
        coordinates
    :param int rank: Number of spatial dimensions
    :return: Property-set created
    :rtype: PropertySet
    :raises RuntimeError: In case the property-set cannot be created
)",
                    py::return_value_policy::reference_internal)

                ;
        }

    }  // namespace data_model
}  // namespace lue
