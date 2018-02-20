#include "lue/space/stationary/point.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace stationary {

void init_point(
    py::module& module)
{

    py::class_<Point, constant_shape::same_shape::ContinuousValue>(
        module,
        "Point",
        "Point docstring...")

        ;

}

}  // namespace stationary
}  // namespace lue
