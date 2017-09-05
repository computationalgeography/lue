#include "lue/hdf5/file.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace hdf5 {

void init_file_class(
        py::module& module)
{

    py::class_<File, Group>(
        module,
        "File",
        R"(
    Class representing an HDF5 file
)")

        .def_property_readonly(
            "pathname",
            &File::pathname,
            R"(
    Return pathname of HDF5 file
)")

        .def(
            "flush",
            &File::flush,
            R"(
    Flush all buffers associated with the file to disk

    This may be necessary when calling external processes from Python
    that have to read the file. Without flushing the buffers such processes
    may not see a valid HDF5 file.
)")

        ;

}

}  // namespace hdf5
}  // namespace lue
