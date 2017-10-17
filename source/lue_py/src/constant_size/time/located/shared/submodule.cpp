#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace constant_size {
namespace time {
namespace located {
namespace shared {
// namespace different_shape {
// 
// void init_submodule(py::module& module);
// 
// }  // namespace different_shape
// 
// 
// namespace same_shape {
// 
// void init_submodule(py::module& module);
// 
// }  // namespace same_shape


// void init_domain_class(py::module& module);
// void init_property_class(py::module& module);
void init_property_set(py::module& module);
void init_time_box(py::module& module);
void init_time_box_domain(py::module& module);
void init_time_domain(py::module& module);


void init_submodule(
    py::module& module)
{
    py::module submodule = module.def_submodule(
        "shared",
        R"(
    Module implementing the API for information that varies through time,
    where these changes happen at the same time for all items
)");

    // init_domain_class(submodule);
    init_time_domain(submodule);
    // init_property_class(submodule);

    // different_shape::init_submodule(submodule);
    // same_shape::init_submodule(submodule);

    init_property_set(submodule);
    init_time_box(submodule);
    init_time_box_domain(submodule);
}

} // namespace shared
} // namespace located
} // namespace time
} // namespace constant_size
} // namespace lue
