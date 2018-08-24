#include "lue/object/phenomena.hpp"
#include "../core/collection.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {

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
            [](
                    Phenomenon const& phenomenon) {
                return "Phenomenon(pathname='" +
                    phenomenon.id().pathname() + "')";
            }
        )

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

        ;

}

}  // namespace lue
