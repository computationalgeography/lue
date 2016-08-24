#include "lue/cxx_api/hdf5/identifier.h"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace python {

void init_identifier(
        py::module& module)
{

    py::class_<hdf5::Identifier>(module, "Identifier",
        "Identifier docstring...")
        .def_property_readonly("id", &hdf5::Identifier::operator hid_t,
            "Return identifier of HDF5 object\n"
            "\n"
            "This identifier can be passed into other HDF5 APIs")
        .def_property_readonly("pathname", &hdf5::Identifier::pathname,
            "Return name of HDF5 object\n"
            "\n"
            "This name can be passed into other HDF5 APIs")
    ;

}

}  // namespace python
}  // namespace lue
