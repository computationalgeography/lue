#include "lue/constant_size/time/located/shared/time_box.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace constant_size {
namespace time {
namespace located {
namespace shared {

void init_time_box(
    py::module& module)
{
    py::class_<TimeBox, omnipresent::same_shape::Value>(
        module,
        "TimeBox",
        "TimeBox...")

        // .def(
        //     "reserve",
        //     &lue::constant_size::time::omnipresent::TimeBox::reserve,
        //     "reserve docstring...",
        //     py::return_value_policy::reference_internal)

        ;

}

}  // namespace shared
}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
