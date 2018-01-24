#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace constant_collection {
namespace constant_shape {
namespace different_shape {

void init_constant(py::module& module);
void init_variable(py::module& module);


void init_submodule(
    py::module& module)
{
    py::module submodule = module.def_submodule(
        "different_shape",
        R"(
    TODO Docstring...
)");

    init_constant(submodule);
    init_variable(submodule);
}

}  // namespace different_shape
}  // namespace constant_shape
}  // namespace constant_collection
}  // namespace lue
