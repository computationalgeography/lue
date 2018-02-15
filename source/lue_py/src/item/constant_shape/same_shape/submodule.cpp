#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace constant_shape {
namespace same_shape {
namespace constant_collection {

void init_submodule(py::module& module);

}  // namespace constant_collection


void init_continuous_value(py::module& module);


void init_submodule(
    py::module& module)
{
    py::module submodule = module.def_submodule(
        "same_shape",
        R"(
    TODO Docstring...

    .. automodule:: lue.constant_shape.same_shape.constant_collection
)");

    init_continuous_value(submodule);

    constant_collection::init_submodule(submodule);
}

}  // namespace same_shape
}  // namespace constant_shape
}  // namespace lue
