#include "lue/object/time/time_domain.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {

void init_time_domain(
    py::module& module)
{

    py::enum_<TimeDomainItemType>(
        module,
        "time_domain_item_type",
        "time_domain_item_type docstring...")
        .value("box", TimeDomainItemType::box)
        .value("point", TimeDomainItemType::point)
        ;


    py::class_<TimeConfiguration>(
        module,
        "TimeConfiguration",
        R"(
    TODO
)")

        .def(
            py::init<TimeDomainItemType>())

        ;

    py::class_<TimeDomain, hdf5::Group>(
        module,
        "TimeDomain",
        R"(
    TODO
)")

        ;

}

}  // namespace lue
