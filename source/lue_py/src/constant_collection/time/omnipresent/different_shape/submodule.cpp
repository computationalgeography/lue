#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace constant_collection {
namespace time {
namespace omnipresent {
namespace different_shape {

void init_value_class(py::module& module);
void init_property_class(py::module& module);


void init_submodule(
    py::module& module)
{
    py::module submodule = module.def_submodule(
        "different_shape",
        R"(
    Module implementing the API for properties with a different shape
    per item
)");

    init_value_class(submodule);
    init_property_class(submodule);
}

}  // namespace different_shape
}  // namespace omnipresent
}  // namespace time
}  // namespace constant_collection
}  // namespace lue
