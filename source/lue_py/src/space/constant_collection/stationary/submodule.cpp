#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace constant_collection {
namespace stationary {

void init_box(py::module& module);
void init_point(py::module& module);


void init_submodule(
    py::module& module)
{
    py::module submodule = module.def_submodule(
        "stationary",
        R"(
    TODO Docstring...
)");

    init_box(submodule);
    init_point(submodule);
}

}  // namespace stationary
}  // namespace constant_collection
}  // namespace lue
