#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace python {
namespace constant_size {
namespace time {

void init_submodule(py::module& module);

}  // namespace time


void init_domain_class(py::module& module);
void init_property_class(py::module& module);
void init_property_set_class(py::module& module);


void init_submodule(
    py::module& module)
{
    py::module submodule = module.def_submodule("constant_size");

    init_domain_class(submodule);
    init_property_class(submodule);
    init_property_set_class(submodule);

    time::init_submodule(submodule);
}

}  // namespace constant_size
}  // namespace python
}  // namespace lue
