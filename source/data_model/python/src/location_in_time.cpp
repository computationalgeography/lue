#include "python_extension.hpp"
#include "lue/info/time/location_in_time.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {

void init_location_in_time(
    py::module& module)
{

    py::class_<LocationInTime, same_shape::constant_shape::Value>(
        module,
        "LocationInTime",
        R"(
    TODO
)")

        ;

}

}  // namespace lue
