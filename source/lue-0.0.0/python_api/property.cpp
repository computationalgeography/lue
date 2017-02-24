#include "lue/python_api/collection.h"
#include "lue/cxx_api/properties.h"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace python {

void init_property(
        py::module& module)
{

    BASE_COLLECTION(Properties<>, Property)

    py::class_<Properties<>, PropertyCollection>(module, "Properties",
        "Properties docstring...")

        .def("add", &Properties<>::add,
            "add docstring...",
            py::return_value_policy::reference_internal)
    ;

    py::class_<Property, hdf5::Group>(module, "Property",
        "Property docstring...")
    ;

}

}  // namespace python
}  // namespace lue
