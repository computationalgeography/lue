#include "lue/py/data_model/core/time/epoch.hpp"
#include "../../python_extension.hpp"
#include "lue/core/aspect.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue::data_model {

    auto formal_string_representation(time::Epoch const& epoch) -> std::string
    {
        std::string result;

        if (!epoch.origin())
        {
            result = std::format("Epoch(kind={})", aspect_to_string(epoch.kind()));
        }
        else
        {
            if (!epoch.calendar())
            {
                result = std::format(
                    "Epoch(kind={}, origin='{}')", aspect_to_string(epoch.kind()), *epoch.origin());
            }
            else
            {
                result = std::format(
                    "Epoch(kind={}, origin='{}', calendar={})",
                    aspect_to_string(epoch.kind()),
                    *epoch.origin(),
                    aspect_to_string(*epoch.calendar()));
            }
        }

        return result;
    }


    auto informal_string_representation(time::Epoch const& epoch) -> std::string
    {
        return formal_string_representation(epoch);
    }


    void init_epoch(py::module& module)
    {

        py::enum_<time::Calendar>(
            module,
            "Calendar",
            R"(
    TODO
)")
            .value(aspect_to_string(time::Calendar::gregorian).c_str(), time::Calendar::gregorian);

        py::class_<time::Epoch> epoch(
            module,
            "Epoch",
            R"(
    Location in time that marks the start of an era

    All time points are at a distance in time (duration) relative to
    an epoch.

    An epoch is defined by an epoch kind, of which there are multiple. An
    epoch kind is a well known point in time, like when the earth
    was formed. As an extension, an origin kan be passed in,
    which defines a location in time after the epoch kind. This allows
    one to define epoch as the start of the current year by using the
    :py:class:`Kind.common_era` epoch kind and a date string formatted
    as an `ISO 8601 string <https://en.wikipedia.org/wiki/ISO_8601>`_. If
    necessary, to be able to interpret the origin string, a calendar
    can be passed in also.

    The origin and calendar are not used internally. They are useful for
    clients that need to translate locations in time to more user-friendly
    formats.
)");

        epoch

            .def(
                py::init<>(),
                R"(
    Default construct an instance

    This instance is set to Unix time epoch: kind is
    :py:class:`Kind.common_era`, origin is "1970-01-01T00:00:00+00:00",
    and calendar is :py:class:`Calendar.gregorian`.
)")

            .def(
                py::init<time::Epoch::Kind>(),
                R"(
    Construct an instance base on epoch kind

    :param Kind kind: Epoch kind
)",
                "kind"_a)

            .def(
                py::init<time::Epoch::Kind, std::string const&>(),
                R"(
    Construct an instance base on epoch kind and origin

    :param Kind kind: Epoch kind
    :param str origin: Location in time after epoch kind
)",
                "kind"_a,
                "origin"_a)

            .def(
                py::init<time::Epoch::Kind, std::string const&, time::Calendar>(),
                R"(
    Construct an instance base on epoch kind, origin and calendar

    :param Kind kind: Epoch kind
    :param str origin: Location in time after epoch kind
    :param str calendar: Calender for interpreting locations in time
)")

            .def(
                "__repr__",
                [](time::Epoch const& epoch) -> std::string { return formal_string_representation(epoch); })

            .def(
                "__str__",
                [](time::Epoch const& epoch) -> std::string { return informal_string_representation(epoch); })

            .def_property_readonly("kind", &time::Epoch::kind)

            .def_property_readonly(
                "origin",
                [](time::Epoch const& epoch) -> py::object
                { return epoch.origin() ? py::cast(*epoch.origin()) : py::none(); })

            .def_property_readonly(
                "calendar",
                [](time::Epoch const& epoch) -> py::object
                { return epoch.calendar() ? py::cast(*epoch.calendar()) : py::none(); })

            ;

        py::enum_<time::Epoch::Kind>(epoch, "Kind")

            .value(aspect_to_string(time::Epoch::Kind::common_era).c_str(), time::Epoch::Kind::common_era)

            .value(
                aspect_to_string(time::Epoch::Kind::formation_of_earth).c_str(),
                time::Epoch::Kind::formation_of_earth)

            ;
    }

}  // namespace lue::data_model
