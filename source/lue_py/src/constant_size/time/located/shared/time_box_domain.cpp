#include "lue/constant_size/time/located/shared/time_box_domain.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace constant_size {
namespace time {
namespace located {
namespace shared {

void init_time_box_domain(
    py::module& module)
{

    py::class_<TimeBoxDomain, TimeDomain>(
        module,
        "TimeBoxDomain",
        "TimeBoxDomain docstring...")

        .def(
            "reserve",
            &TimeBoxDomain::reserve,
            "reserve docstring...",
            py::return_value_policy::reference_internal)

        .def_property_readonly(
            "items",
            py::overload_cast<>(&TimeBoxDomain::items),
            "items docstring...",
            py::return_value_policy::reference_internal)

        ;


    module.def(
        "create_time_box_domain",
        &create_time_box_domain,
        R"(
    Create new time box domain

    The domain will be added to the domain of the property set
)",
        "property_set"_a,
        "clock"_a,
        py::return_value_policy::move)

        ;

}

}  // namespace shared
}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
