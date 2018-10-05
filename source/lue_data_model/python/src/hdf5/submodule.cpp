#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace hdf5 {

void init_dataset(py::module& module);
// void init_datatype(py::module& module);
void init_file(py::module& module);
void init_group(py::module& module);
void init_identifier(py::module& module);
void init_shape(py::module& module);
void init_validate(py::module& module);


void init_submodule(
    py::module& module)
{
    py::module submodule = module.def_submodule(
        "hdf5",
        R"(
    :mod:`lue.hdf5` --- Manipulating HDF5 from Python
    =================================================

    The :mod:`lue.hdf5` package wraps the LUE C++ API that, in turn, is
    implemented on top of the HDF5 C API. In general you don't need to use
    this package. It exists because it is a side effect of implementing
    the rest of the :mod:`lue` package. It may be useful in case you
    don't need the functionality offered by the :mod:`lue` package,
    or if you need functionality that is not offered by this package yet.
)");

    init_shape(submodule);
    init_identifier(submodule);
    init_group(submodule);
    init_file(submodule);
    // init_datatype(submodule);
    init_dataset(submodule);
    init_validate(submodule);
}

}  // namespace hdf5
}  // namespace lue
