#include "lue/space/stationary/box.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace stationary {

void init_box(
    py::module& module)
{

    py::class_<Box, same_shape::constant_shape::ContinuousValue>(
        module,
        "Box",
        "Box docstring...")

        ;

}

}  // namespace stationary
}  // namespace lue
