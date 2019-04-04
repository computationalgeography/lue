#include "../../python_extension.hpp"
#include "lue/py/core/time/epoch.hpp"
#include "lue/core/aspect.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {

std::string formal_string_representation(
    time::Epoch const& epoch)
{
    std::string result;

    if(!epoch.origin()) {
        result = fmt::format(
            "Epoch(kind={})",
            aspect_to_string(epoch.kind()));
    }
    else {
        if(!epoch.calendar()) {
            result = fmt::format(
                "Epoch(kind={}, origin='{}')",
                aspect_to_string(epoch.kind()),
                *epoch.origin());
        }
        else {
            result = fmt::format(
                "Epoch(kind={}, origin='{}', calendar={})",
                aspect_to_string(epoch.kind()),
                *epoch.origin(),
                aspect_to_string(*epoch.calendar()));
        }
    }

    return result;
}


std::string informal_string_representation(
    time::Epoch const& epoch)
{
    return formal_string_representation(epoch);
}


void init_epoch(
    py::module& module)
{

    py::enum_<time::Calendar>(
        module,
        "Calendar",
        R"(
    TODO
)")
        .value(
            aspect_to_string(time::Calendar::gregorian).c_str(),
            time::Calendar::gregorian)
        ;

    py::class_<time::Epoch> epoch(
        module,
        "Epoch",
        R"(
    TODO
)");

    epoch

        .def(
            py::init<time::Epoch::Kind, std::string const&, time::Calendar>())

        .def(
            "__repr__",
            [](time::Epoch const& epoch) {
                return formal_string_representation(epoch);
            }
        )

        .def(
            "__str__",
            [](time::Epoch const& epoch) {
                return informal_string_representation(epoch);
            }
        )

        .def_property_readonly(
            "kind",
            &time::Epoch::kind)

        .def_property_readonly(
            "origin",
            [](time::Epoch const& epoch) {
                return epoch.origin()
                    ? py::cast<std::string>(*epoch.origin())
                    : py::none()
                    ;
            })

        .def_property_readonly(
            "calendar",
            [](time::Epoch const& epoch) {
                return epoch.calendar()
                    ? py::cast<time::Calendar>(*epoch.calendar())
                    : py::none()
                    ;
            })

        ;

    py::enum_<time::Epoch::Kind>(
        epoch,
        "Kind",
        R"(
    TODO
)")

        .value(
            aspect_to_string(time::Epoch::Kind::anno_domini).c_str(),
            time::Epoch::Kind::anno_domini)

        .value(
            aspect_to_string(time::Epoch::Kind::formation_of_earth).c_str(),
            time::Epoch::Kind::formation_of_earth)

        ;

}

}  // namespace lue
