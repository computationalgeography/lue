#include "lue/cxx_api/universes.h"
#include "lue/python_api/collection.h"
#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace python {

void init_universe(
        py::module& module)
{

    BASE_COLLECTION(Universe)

    py::class_<Universes>(module, "Universes", py::base<UniverseCollection>(),
        "Universes docstring...")
        .def("add", &Universes::add,
            "add docstring...",
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
