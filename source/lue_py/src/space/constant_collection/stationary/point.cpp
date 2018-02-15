#include "lue/space/constant_collection/stationary/point.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace constant_collection {
namespace stationary {

void init_point(
    py::module& module)
{

    py::class_<Point, lue::constant_shape::same_shape::ContinuousValue>(
        module,
        "Point",
        "Point docstring...")

        ;

}

}  // namespace stationary
}  // namespace constant_collection
}  // namespace lue
