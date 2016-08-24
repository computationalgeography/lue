#include "lue/cxx_api/universes.h"
#include "lue/python_api/collection.h"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace python {

void init_universe(
        py::module& module)
{

    BASE_COLLECTION(Universe)

    py::class_<Universes>(module, "Universes", py::base<UniverseCollection>(),
        R"(Collection of LUE universes

    Zero of more Universe instances can be stored in a Universe
    collection.

    It is not possible to create Universes instances from scratch:
    a constructor is not defined. Universe collections can be obtained
    from Dataset instances.
)")
        .def("add", &Universes::add,
            R"(Add new universe to collection

    :param str name: Name of universe to create
    :raises RuntimeError: In case the universe cannot be created
)",
            "name"_a,
            py::return_value_policy::reference_internal)
    ;


    py::class_<Universe>(module, "Universe", py::base<hdf5::Group>(),
        "Universe docstring...")
        .def("add_phenomenon", &Universe::add_phenomenon,
            "add_phenomenon docstring...",
            py::return_value_policy::reference_internal)
        .def_property_readonly("phenomena", &Universe::phenomena,
            "phenomena docstring...",
            py::return_value_policy::reference_internal)
    ;

}

}  // namespace python
}  // namespace lue
