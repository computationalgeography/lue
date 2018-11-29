#include "../python_extension.hpp"
#include "lue/object/phenomena.hpp"
#include "../core/collection.hpp"
#include <pybind11/pybind11.h>
#include <boost/algorithm/string/join.hpp>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace {

static std::string formal_string_representation(
    Phenomenon const& phenomenon)
{
    return fmt::format(
        "Phenomenon(pathname='{}')",
        phenomenon.id().pathname());
}


static std::string informal_string_representation(
    Phenomenon const& phenomenon)
{
    return fmt::format(
        "{}\n"
        "    collection-property-sets: [{}]\n"
        "    property-sets: [{}]",
        formal_string_representation(phenomenon),
        boost::algorithm::join(
            phenomenon.collection_property_sets().names(), ", "),
        boost::algorithm::join(phenomenon.property_sets().names(), ", "));
}

}  // Anonymous namespace


void init_phenomenon(
    py::module& module)
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

        .def("add",
            &Phenomena::add,
            "name"_a,
            R"(
    Add new phenomenon to collection

    :param str name: Name of phenomenon to create
    :return: Phenomenon created
    :rtype: lue.Phenomenon
    :raises RuntimeError: In case the phenomenon cannot be created
)",
            py::return_value_policy::reference_internal)

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
            [](Phenomenon const& phenomenon) {
                return formal_string_representation(phenomenon);
            }
        )

        .def(
            "__str__",
            [](Phenomenon const& phenomenon) {
                return informal_string_representation(phenomenon);
            }
        )

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

    :rtype: lue.PropertySets
)",
            py::return_value_policy::reference_internal)

        .def_property_readonly(
            "collection_property_sets",
            py::overload_cast<>(&Phenomenon::collection_property_sets),
            R"(
    Return property sets collection containing information about the
    collection of objects

    :rtype: lue.PropertySets
)",
            py::return_value_policy::reference_internal)

        .def(
            "__getattr__",
            [](
                Phenomenon& phenomenon,
                std::string const& property_set_name)
            {
                auto const property_set_found =
                    phenomenon.property_sets().contains(property_set_name);
                auto const collection_property_set_found =
                    phenomenon.collection_property_sets().contains(
                        property_set_name);


                if(!property_set_found && !collection_property_set_found) {
                    // TODO We are throwing a KeyError here. Should be
                    // an AttributeError, but pybind11 does not seem to
                    // support that yet.
                    //
                    // Python message:
                    // AttributeError: 'x' object has no attribute 'y'
                    // Ours is a little bit different:
                    throw pybind11::key_error(fmt::format(
                        "Phenomenon does not contain (collection) "
                        "property-set '{}'",
                        property_set_name));
                }

                assert(property_set_found || collection_property_set_found);

                auto& property_sets = property_set_found
                    ? phenomenon.property_sets()
                    : phenomenon.collection_property_sets()
                    ;

                return py::cast(&property_sets[property_set_name]);
            },
            py::return_value_policy::reference_internal)

        ;

}

}  // namespace lue
