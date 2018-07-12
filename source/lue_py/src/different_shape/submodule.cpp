#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace different_shape {

void init_properties(py::module& module);
void init_property(py::module& module);
void init_value(py::module& module);


namespace constant_shape {

void init_submodule(py::module& module);

}  // namespace constant_shape


namespace variable_shape {

void init_submodule(py::module& module);

}  // namespace variable_shape


void init_submodule(
    py::module& module)
{
    py::module submodule = module.def_submodule(
        "different_shape",
        R"(
    TODO
)");

    init_properties(submodule);
    init_property(submodule);
    init_value(submodule);

    constant_shape::init_submodule(submodule);
    variable_shape::init_submodule(submodule);
}

}  // namespace different_shape
}  // namespace lue
