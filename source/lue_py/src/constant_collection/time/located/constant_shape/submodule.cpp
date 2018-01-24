#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace constant_collection {
namespace time {
namespace located {
namespace constant_shape {
namespace different_shape {

void init_submodule(py::module& module);

}  // namespace different_shape


namespace same_shape {

void init_submodule(py::module& module);

}  // namespace same_shape


void init_property(py::module& module);


void init_submodule(
    py::module& module)
{
    py::module submodule = module.def_submodule(
        "constant_shape",
        R"(
    TODO
)");

    constant_shape::init_property(submodule);
    constant_shape::different_shape::init_submodule(submodule);
    constant_shape::same_shape::init_submodule(submodule);
}

} // namespace constant_shape
} // namespace located
} // namespace time
} // namespace constant_collection
} // namespace lue
