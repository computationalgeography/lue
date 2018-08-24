#include "lue/core/clock.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {

void init_clock(
    py::module& module)
{

    py::enum_<time::Unit>(
        module,
        "Unit",
        R"(
    Time units determine the smallest possible resolution of a
    :class:`clock <Clock>`

    The actual resolution also determines on the number of units stored in
    the clock.
)")
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
        R"(
    Class for representing clocks

    Locations in time are stored using a number of ticks. The amount of
    time a single tick represents is determined by the clock. Examples
    of tick periods are 1 second, 7 days, and 2 years.

    :param Unit unit: Unit of time
    :param int count: Number of units of time in a single tick

    .. note::

        Epoch is needed to position locations in time in time. Currently,
        epochs are not supported yet.
)")

        .def(
            py::init<time::Unit const, time::TickPeriodCount const>())

        ;

}

}  // namespace lue
