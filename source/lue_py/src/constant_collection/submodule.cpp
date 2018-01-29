#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace constant_collection {
namespace constant_shape {

// Implementation in item/ tree
void init_submodule(py::module& module);

}  // namespace constant_shape

namespace stationary {

// Implementation in space/ tree
void init_submodule(py::module& module);

}


namespace time {

void init_submodule(py::module& module);

}  // namespace time


void init_domain_class(py::module& module);
void init_property_class(py::module& module);
void init_property_set_class(py::module& module);
void init_space_domain_class(py::module& module);


void init_submodule(
    py::module& module)
{
    py::module submodule = module.def_submodule(
        "constant_collection",
        R"(
    Module implementing the API for property sets with a constant number
    of items

    .. automodule:: lue.constant_collection.constant_shape
    .. automodule:: lue.constant_collection.stationary
    .. automodule:: lue.constant_collection.time
)");
    init_domain_class(submodule);
    init_space_domain_class(submodule);
    init_property_class(submodule);
    init_property_set_class(submodule);

    constant_shape::init_submodule(submodule);
    stationary::init_submodule(submodule);
    time::init_submodule(submodule);
}

}  // namespace constant_collection
}  // namespace lue
