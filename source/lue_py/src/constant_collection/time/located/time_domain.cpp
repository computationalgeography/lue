#include "lue/constant_collection/time/located/time_domain.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace constant_collection {
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
}  // namespace constant_collection
}  // namespace lue
