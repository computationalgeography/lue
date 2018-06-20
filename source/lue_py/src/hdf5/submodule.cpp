#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace hdf5 {

void init_dataset(py::module& module);
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
    Python extension wrapping the LUE C++ API around the HDF5 C API

    The LUE C++ API consists of thin wrappers around the HDF5 C API. This
    Python extension consists of thin wrappers around the LUE C++ API. It
    can be used on its own, or when working with LUE datasets, when the
    higher level APIs don't suffice.
)");

    init_shape(submodule);
    init_identifier(submodule);
    init_group(submodule);
    init_file(submodule);
    init_dataset(submodule);
    init_validate(submodule);
}

}  // namespace hdf5
}  // namespace lue
