#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace constant_size {
namespace constant {
namespace same_shape {

void init_collection(py::module& module);

}  // namespace same_shape
}  // namespace constant


namespace time {
namespace omnipresent {
namespace same_shape {

void init_value_class(py::module& module);
void init_property_class(py::module& module);


void init_submodule(
    py::module& module)
{
    py::module submodule = module.def_submodule(
        "same_shape",
        R"(
    Module implementing the API for properties with the same shape per item
)");

    constant::same_shape::init_collection(submodule);
    init_value_class(submodule);
    init_property_class(submodule);
}

}  // namespace same_shape
}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
