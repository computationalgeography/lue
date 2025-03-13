#include "lue/core/clock.hpp"
#include "../python_extension.hpp"
#include "lue/core/aspect.hpp"
#include "lue/py/data_model/core/time/epoch.hpp"
#include <pybind11/pybind11.h>
#include <format>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
    namespace data_model {

        std::string informal_string_representation(time::Unit const unit)
        {
            return aspect_to_string(unit);
        }


        std::string formal_string_representation(Clock const& clock)
        {
            return std::format(
                "Clock(epoch={}, unit={}, nr_units={})",
                formal_string_representation(clock.epoch()),
                aspect_to_string(clock.unit()),
                clock.nr_units());
        }


        std::string informal_string_representation(Clock const& clock)
        {
            return formal_string_representation(clock);
        }


        void init_clock(py::module& module)
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
                .value("nanosecond", time::Unit::nanosecond)
                .value("microsecond", time::Unit::microsecond)
                .value("millisecond", time::Unit::millisecond)
                .value("second", time::Unit::second)
                .value("minute", time::Unit::minute)
                .value("hour", time::Unit::hour)
                .value("day", time::Unit::day)
                .value("week", time::Unit::week)
                .value("month", time::Unit::month)
                .value("year", time::Unit::year)

                .def("__str__", [](time::Unit const unit) { return informal_string_representation(unit); })

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

                .def(py::init<time::Unit const, time::TickPeriodCount const>())
                .def(py::init<time::Epoch const&, time::Unit const, time::TickPeriodCount const>())

                .def("__repr__", [](Clock const& clock) { return formal_string_representation(clock); })

                .def("__str__", [](Clock const& clock) { return informal_string_representation(clock); })

                .def_property_readonly("epoch", &Clock::epoch)

                .def_property_readonly("unit", &Clock::unit)

                .def_property_readonly("nr_units", &Clock::nr_units)

                ;
        }

    }  // namespace data_model
}  // namespace lue
