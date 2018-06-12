#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace different_shape {
namespace constant_shape {
namespace constant_collection {

void init_submodule(py::module& module);

}  // namespace constant_collection




void init_submodule(
    py::module& module)
{
    py::module submodule = module.def_submodule(
        "different_shape",
        R"(
    TODO Docstring...

    .. automodule:: lue.constant_shape.different_shape.constant_collection
)");

    constant_collection::init_submodule(submodule);
}

}  // namespace constant_shape
}  // namespace different_shape
}  // namespace lue
