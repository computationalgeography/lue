#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace constant_collection {
namespace time {
namespace located {
namespace constant_shape {

void init_submodule(py::module& module);

}  // namespace constant_shape


void init_time_domain(py::module& module);
void init_property(py::module& module);

void init_space_box_domain_class(py::module& module);
void init_time_box_domain(py::module& module);


void init_submodule(
    py::module& module)
{
    py::module submodule = module.def_submodule(
        "located",
        R"(
    Module implementing the API for information that varies through time
)");

    init_space_box_domain_class(submodule);

    init_time_domain(submodule);
    init_time_box_domain(submodule);

    init_property(submodule);

    constant_shape::init_submodule(submodule);
}

} // namespace located
} // namespace time
} // namespace constant_collection
} // namespace lue
