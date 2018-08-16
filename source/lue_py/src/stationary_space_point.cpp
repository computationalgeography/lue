#include "lue/info/space/stationary_space_point.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {

void init_stationary_space_point(
    py::module& module)
{

    py::class_<StationarySpacePoint, same_shape::Value>(
        module,
        "StationarySpacePoint",
        R"(
    TODO
)")

        ;

}

}  // namespace lue
