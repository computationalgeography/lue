#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace constant_shape {
namespace different_shape {
namespace constant_collection {

void init_synchronous_value(py::module& module);


void init_submodule(
    py::module& module)
{
    py::module submodule = module.def_submodule(
        "constant_collection",
        R"(
    TODO Docstring...
)");

    init_synchronous_value(submodule);
}

}  // namespace constant_collection
}  // namespace different_shape
}  // namespace constant_shape
}  // namespace lue
