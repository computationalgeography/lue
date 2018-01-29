#include "lue/space/constant_collection/stationary/box.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace constant_collection {
namespace stationary {

void init_box(
    py::module& module)
{

    py::class_<Box, constant_shape::same_shape::Constant>(
        module,
        "Box",
        "Box docstring...")

        ;

}

}  // namespace stationary
}  // namespace constant_collection
}  // namespace lue
