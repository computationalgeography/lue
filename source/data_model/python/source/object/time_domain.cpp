#include "lue/object/time_domain.hpp"
#include "../python_extension.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
    namespace data_model {
        namespace {

            static std::string formal_string_representation(TimeConfiguration const& configuration)
            {
                return fmt::format(
                    "TimeConfiguration(item_type='{}')",
                    aspect_to_string(configuration.value<TimeDomainItemType>()));
            }


            static std::string informal_string_representation(TimeConfiguration const& configuration)
            {
                return formal_string_representation(configuration);
            }


            static std::string formal_string_representation(TimeDomain const& domain)
            {
                return fmt::format("TimeDomain(pathname='{}')", domain.id().pathname());
            }


            static std::string informal_string_representation(TimeDomain const& domain)
            {
                return formal_string_representation(domain);
            }

        }  // Anonymous namespace


        void init_time_domain(py::module& module)
        {

            py::enum_<TimeDomainItemType>(
                module,
                "TimeDomainItemType",
                R"(
    Locations in time can be represented by boxes, cells or points
)")
                .value("box", TimeDomainItemType::box)
                .value("cell", TimeDomainItemType::cell)
                .value("point", TimeDomainItemType::point);


            py::class_<TimeConfiguration>(
                module,
                "TimeConfiguration",
                R"(
    Class for representing the aspects of the time domain
)")

                .def(py::init<TimeDomainItemType>())

                .def(
                    "__repr__",
                    [](TimeConfiguration const& configuration)
                    { return formal_string_representation(configuration); })

                .def(
                    "__str__",
                    [](TimeConfiguration const& configuration)
                    { return informal_string_representation(configuration); })

                .def_property_readonly("item_type", &TimeConfiguration::value<TimeDomainItemType>)

                ;

            py::class_<TimeDomain, hdf5::Group>(
                module,
                "TimeDomain",
                R"(
    TODO
)")

                .def(
                    "__repr__", [](TimeDomain const& domain) { return formal_string_representation(domain); })

                .def(
                    "__str__",
                    [](TimeDomain const& domain) { return informal_string_representation(domain); })

                .def_property_readonly("configuration", &TimeDomain::configuration)

                .def_property_readonly("clock", &TimeDomain::clock)

                .def_property_readonly(
                    "value",
                    [](TimeDomain& time_domain)
                    {
                        py::object collection = py::none();

                        auto const& configuration = time_domain.configuration();

                        switch (configuration.value<TimeDomainItemType>())
                        {
                            case TimeDomainItemType::box:
                            {
                                using Value = TimeBox;
                                collection = py::cast(time_domain.value<Value>());
                                break;
                            }
                            case TimeDomainItemType::cell:
                            {
                                using Value = TimeCell;
                                collection = py::cast(time_domain.value<Value>());
                                break;
                            }
                            case TimeDomainItemType::point:
                            {
                                using Value = TimePoint;
                                collection = py::cast(time_domain.value<Value>());
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

    }  // namespace data_model
}  // namespace lue
