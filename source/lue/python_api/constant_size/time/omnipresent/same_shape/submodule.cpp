#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace python {
namespace constant_size {
namespace time {
namespace omnipresent {
namespace same_shape {


void init_item_class(py::module& module);
void init_property_class(py::module& module);


void init_submodule(
    py::module& module)
{
    py::module submodule = module.def_submodule("same_shape");

    init_item_class(submodule);
    init_property_class(submodule);
}

}  // namespace same_shape
}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace python
}  // namespace lue
