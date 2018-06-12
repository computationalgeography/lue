#include "lue/clock.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {

void init_clock(
    py::module& module)
{

    py::enum_<time::Unit>(
        module,
        "unit",
        "unit docstring...")
        .value("second", time::Unit::second)
        .value("minute", time::Unit::minute)
        .value("hour", time::Unit::hour)
        .value("day", time::Unit::day)
        .value("week", time::Unit::week)
        .value("month", time::Unit::month)
        .value("year", time::Unit::year)
        ;

    py::class_<Clock>(
        module,
        "Clock",
        "Clock docstring...")

        .def(py::init<time::Unit const, time::TickPeriodCount const>())

        ;

}

}  // namespace lue
