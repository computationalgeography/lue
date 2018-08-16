#include "lue/info/time/time_box.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {

void init_time_box(
    py::module& module)
{

    py::class_<TimeBox, same_shape::constant_shape::Value>(
        module,
        "TimeBox",
        R"(
    TODO
)")

        ;

}

}  // namespace lue
