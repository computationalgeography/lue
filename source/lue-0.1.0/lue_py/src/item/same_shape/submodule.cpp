#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace same_shape {
namespace constant_shape {

void init_submodule(py::module& module);

}  // namespace constant_shape


void init_continuous_value(py::module& module);


void init_submodule(
    py::module& module)
{
    py::module submodule = module.def_submodule(
        "same_shape",
        R"(
    TODO Docstring

    .. automodule:: lue.same_shape.constant_shape
)");

    init_continuous_value(submodule);

    constant_shape::init_submodule(submodule);
}

}  // namespace same_shape
}  // namespace lue
