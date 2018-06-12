#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace hdf5 {

void init_dataset_class(py::module& module);
void init_file_class(py::module& module);
void init_group_class(py::module& module);
void init_identifier_class(py::module& module);
void init_shape_class(py::module& module);
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

    init_shape_class(submodule);
    init_identifier_class(submodule);
    init_group_class(submodule);
    init_file_class(submodule);
    init_dataset_class(submodule);
    init_validate(submodule);
}

}  // namespace hdf5
}  // namespace lue
