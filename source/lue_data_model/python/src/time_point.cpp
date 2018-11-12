#include "python_extension.hpp"
#include "lue/info/time/time_point.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {

void init_time_point(
    py::module& module)
{

    py::class_<TimePoint, same_shape::constant_shape::Value>(
        module,
        "TimePoint",
        R"(
    TODO
)")

        ;

}

}  // namespace lue
