#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace constant_size {
namespace variable {
namespace constant_shape {

void init_submodule(py::module& module);

}  // namespace constant_shape


void init_submodule(
    py::module& module)
{
    py::module submodule = module.def_submodule(
        "variable",
        R"(
    TODO Docstring

    .. automodule:: lue.constant_size.variable.constant_shape
)");

    constant_shape::init_submodule(submodule);
}

}  // namespace variable
}  // namespace constant_size
}  // namespace lue
