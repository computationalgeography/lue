#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace constant_size {
namespace variable {
namespace constant_shape {
namespace different_shape {

void init_submodule(py::module& module);

}  // namespace different_shape


namespace same_shape {

void init_submodule(py::module& module);

}  // namespace same_shape


void init_submodule(
    py::module& module)
{
    py::module submodule = module.def_submodule(
        "constant_shape",
        R"(
    TODO Docstring

    .. automodule:: lue.constant_size.variable.constant_shape.different_shape
    .. automodule:: lue.constant_size.variable.constant_shape.same_shape
)");

    different_shape::init_submodule(submodule);
    same_shape::init_submodule(submodule);
}

}  // namespace constant_shape
}  // namespace variable
}  // namespace constant_size
}  // namespace lue
