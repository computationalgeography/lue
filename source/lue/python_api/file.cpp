#include "lue/cxx_api/hdf5/file.h"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace python {

void init_file(
        py::module& module)
{

    py::class_<hdf5::File>(module, "File",
        R"(Class representing an HDF5 file)")
        .def_property_readonly("id", &hdf5::File::id,
            "id docstring...",
            py::return_value_policy::reference_internal)
        .def_property_readonly("pathname", &hdf5::File::pathname,
            "Return pathname of file\n"
            "\n"
            "...")
    ;

}

}  // namespace python
}  // namespace lue
