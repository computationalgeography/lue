#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace constant_collection {
namespace constant_shape {
namespace same_shape {

void init_constant(py::module& module);
void init_synchronous_variable(py::module& module);


void init_submodule(
    py::module& module)
{
    py::module submodule = module.def_submodule(
        "same_shape",
        R"(
    TODO Docstring...
)");

    init_constant(submodule);
    init_synchronous_variable(submodule);
}

}  // namespace same_shape
}  // namespace constant_shape
}  // namespace constant_collection
}  // namespace lue
