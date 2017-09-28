#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {

void init_submodule(py::module& module);

}  // namespace omnipresent


void init_submodule(
    py::module& module)
{
    py::module submodule = module.def_submodule(
        "time",
        R"(
    Parent module for modules implementing the different ways to handle time

    .. automodule:: lue.constant_size.time.omnipresent
)");

    omnipresent::init_submodule(submodule);
}

}  // namespace time
}  // namespace constant_size
}  // namespace lue
