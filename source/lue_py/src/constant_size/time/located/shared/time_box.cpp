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
    py::class_<TimeBox, constant_size::SameShape>(
        module,
        "TimeBox",
        "TimeBox...")

        ;

}

}  // namespace shared
}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
