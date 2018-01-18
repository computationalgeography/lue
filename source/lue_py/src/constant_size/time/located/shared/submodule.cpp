#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace constant_size {
namespace time {
namespace located {
namespace shared {
namespace constant_shape {

void init_submodule(py::module& module);

}  // namespace constant_shape


void init_submodule(
    py::module& module)
{
    py::module submodule = module.def_submodule(
        "shared",
        R"(
    Module implementing the API for information that varies through time,
    where these changes happen at the same time for all items
)");

    constant_shape::init_submodule(submodule);
}

} // namespace shared
} // namespace located
} // namespace time
} // namespace constant_size
} // namespace lue
