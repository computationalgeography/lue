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
        "TimeDomainItemType",
        R"(
    Locations in time can be represented by boxes or points
)")
        .value("box", TimeDomainItemType::box)
        .value("point", TimeDomainItemType::point)
        ;


    py::class_<TimeConfiguration>(
        module,
        "TimeConfiguration",
        R"(
    Class for representing the aspects of the time domain
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

        .def_property_readonly(
            "value",
            [](
                TimeDomain& time_domain)
            {
                py::object collection = py::none();

                auto const& configuration = time_domain.configuration();

                switch(configuration.value<TimeDomainItemType>()) {
                    case TimeDomainItemType::box: {
                        using Value = TimeBox;
                        collection = py::cast(new Value(
                            time_domain.value<Value>()));
                        break;
                    }
                    case TimeDomainItemType::cell: {
                        using Value = TimeCell;
                        collection = py::cast(new Value(
                            time_domain.value<Value>()));
                        break;
                    }
                    case TimeDomainItemType::point: {
                        using Value = TimePoint;
                        collection = py::cast(new Value(
                            time_domain.value<Value>()));
                        break;
                    }
                }

                return collection;
            },
            R"(
    Return instance representing the collection of time domain items
)")

        ;

}

}  // namespace lue
