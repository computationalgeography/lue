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
    Collection of LUE phenomena

    Zero of more Phenomenon instances can be stored in a Phenomenon
    collection.

    It is not possible to create Phenomena instances from scratch:
    a constructor is not defined. Phenomenon collections can be obtained
    from Dataset instances.
)")

        .def("add",
            &Phenomena::add,
            R"(
    Add new phenomenon to collection

    :param str name: Name of phenomenon to create
    :raises RuntimeError: In case the phenomenon cannot be created
)",
            "name"_a,
            py::return_value_policy::reference_internal)

        ;


    py::class_<Phenomenon, hdf5::Group>(
        module,
        "Phenomenon",
        R"(
    Phenomenon docstring...
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
    Return property sets collection

    :rtype: lue.PropertySets
)",
            py::return_value_policy::reference_internal)

        .def_property_readonly(
            "collection_property_sets",
            py::overload_cast<>(&Phenomenon::collection_property_sets),
            R"(
    Return collection property sets collection

    :rtype: lue.PropertySets
)",
            py::return_value_policy::reference_internal)

        ;

}

}  // namespace lue
