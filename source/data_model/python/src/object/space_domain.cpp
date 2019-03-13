#include "../python_extension.hpp"
#include "lue/object/space/space_domain.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace {

static std::string formal_string_representation(
    SpaceConfiguration const& configuration)
{
    return fmt::format(
            "SpaceConfiguration(mobility='{}', item_type='{}')",
            aspect_to_string(configuration.value<Mobility>()),
            aspect_to_string(configuration.value<SpaceDomainItemType>())
        );
}


static std::string informal_string_representation(
    SpaceConfiguration const& configuration)
{
    return formal_string_representation(configuration);
}


static std::string formal_string_representation(
    SpaceDomain const& domain)
{
    return fmt::format(
            "SpaceDomain(pathname='{}')",
            domain.id().pathname()
        );
}


static std::string informal_string_representation(
    SpaceDomain const& domain)
{
    return formal_string_representation(domain);
}

}  // Anonymous namespace


void init_space_domain(
    py::module& module)
{

    py::enum_<Mobility>(
        module,
        "Mobility",
        R"(
    Objects can be stationary in space or mobile
)")
        .value("mobile", Mobility::mobile)
        .value("stationary", Mobility::stationary)
        ;


    py::enum_<SpaceDomainItemType>(
        module,
        "SpaceDomainItemType",
        R"(
    Locations in space can be represented by boxes or points
)")
        .value("box", SpaceDomainItemType::box)
        .value("point", SpaceDomainItemType::point)
        ;


    py::class_<SpaceConfiguration>(
        module,
        "SpaceConfiguration",
        R"(
    Class for representing the aspects of the spatial domain
)")

        .def(
            py::init<Mobility, SpaceDomainItemType>())

        .def(
            "__repr__",
            [](SpaceConfiguration const& configuration) {
                return formal_string_representation(configuration);
            }
        )

        .def(
            "__str__",
            [](SpaceConfiguration const& configuration) {
                return informal_string_representation(configuration);
            }
        )

        .def_property_readonly(
            "mobility",
            &SpaceConfiguration::value<Mobility>)

        .def_property_readonly(
            "item_type",
            &SpaceConfiguration::value<SpaceDomainItemType>)

        ;

    py::class_<SpaceDomain, hdf5::Group>(
        module,
        "SpaceDomain",
        R"(
    TODO
)")

        .def(
            "__repr__",
            [](SpaceDomain const& domain) {
                return formal_string_representation(domain);
            }
        )

        .def(
            "__str__",
            [](SpaceDomain const& domain) {
                return informal_string_representation(domain);
            }
        )

        .def(
            "set_presence_discretization",
            &SpaceDomain::set_presence_discretization)

        .def_property_readonly(
            "presence_is_discretized",
            &SpaceDomain::presence_is_discretized)

        .def_property_readonly(
            "discretized_presence_property",
            &SpaceDomain::discretized_presence_property)

        .def_property_readonly(
            "configuration",
            &SpaceDomain::configuration)

        .def_property_readonly(
            "value",
            [](
                SpaceDomain& space_domain)
            {
                py::object collection = py::none();

                auto const& configuration = space_domain.configuration();

                switch(configuration.value<Mobility>()) {
                    case Mobility::stationary: {
                        switch(configuration.value<SpaceDomainItemType>()) {
                            case SpaceDomainItemType::point: {
                                using Value = StationarySpacePoint;
                                collection = py::cast(new Value(
                                    space_domain.value<Value>()));
                                break;
                            }
                            case SpaceDomainItemType::box: {
                                using Value = StationarySpaceBox;
                                collection = py::cast(new Value(
                                    space_domain.value<Value>()));
                                break;
                            }
                        }

                        break;
                    }
                    case Mobility::mobile: {
                        switch(configuration.value<SpaceDomainItemType>()) {
                            case SpaceDomainItemType::point: {
                                using Value = MobileSpacePoint;
                                collection = py::cast(new Value(
                                    space_domain.value<Value>()));
                                break;
                            }
                            case SpaceDomainItemType::box: {
                                using Value = MobileSpaceBox;
                                collection = py::cast(new Value(
                                    space_domain.value<Value>()));
                                break;
                            }
                        }

                        break;
                    }
                }

                return collection;
            },
            R"(
    Return instance representing the collection of space domain items
)")

        ;

}

}  // namespace lue
