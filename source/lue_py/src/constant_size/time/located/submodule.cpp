#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace constant_size {
namespace time {
namespace located {
namespace shared {

void init_submodule(py::module& module);

}  // namespace shared


void init_domain(py::module& module);
void init_time_domain(py::module& module);
void init_property(py::module& module);
void init_property_set(py::module& module);


void init_submodule(
    py::module& module)
{
    py::module submodule = module.def_submodule(
        "located",
        R"(
    Module implementing the API for information that varies through time
)");

    init_domain(submodule);
    init_time_domain(submodule);
    init_property(submodule);
    init_property_set(submodule);
    shared::init_submodule(submodule);
}

} // namespace located
} // namespace time
} // namespace constant_size
} // namespace lue
