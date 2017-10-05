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
    py::module submodule = module.def_submodule("hdf5");

    init_shape_class(submodule);
    init_identifier_class(submodule);
    init_group_class(submodule);
    init_file_class(submodule);
    init_dataset_class(submodule);
    init_validate(submodule);
}

}  // namespace hdf5
}  // namespace lue
