#include "lue/constant_size/time/located/time_domain.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace constant_size {
namespace time {
namespace located {

void init_time_domain(
    py::module& module)
{

    py::class_<TimeDomain, lue::TimeDomain>(
        module,
        "TimeDomain",
        "TimeDomain docstring...")

        ;

}

}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
