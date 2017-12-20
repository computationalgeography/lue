#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace constant_size {
namespace constant {
namespace different_shape {

void init_collection(py::module& module);


void init_submodule(
    py::module& module)
{
    py::module submodule = module.def_submodule(
        "different_shape",
        R"(
    TODO Docstring...
)");

    init_collection(submodule);
}

}  // namespace different_shape
}  // namespace constant
}  // namespace constant_size
}  // namespace lue
