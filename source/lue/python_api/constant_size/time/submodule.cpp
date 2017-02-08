#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace python {
namespace constant_size {
namespace time {
namespace omnipresent {

void init_submodule(py::module& module);

}  // namespace omnipresent


void init_property_set_class(py::module& module);


void init_submodule(
    py::module& module)
{
    py::module submodule = module.def_submodule("time");

    omnipresent::init_submodule(submodule);
}

}  // namespace time
}  // namespace constant_size
}  // namespace python
}  // namespace lue
