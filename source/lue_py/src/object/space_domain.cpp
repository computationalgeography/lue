#include "lue/object/space/space_domain.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {

void init_space_domain(
    py::module& module)
{

    py::enum_<Mobility>(
        module,
        "Mobility",
        "mobility docstring...")
        .value("mobile", Mobility::mobile)
        .value("stationary", Mobility::stationary)
        ;

    py::enum_<SpaceDomainItemType>(
        module,
        "SpaceDomainItemType",
        "space_domain_item_type docstring...")
        .value("box", SpaceDomainItemType::box)
        .value("point", SpaceDomainItemType::point)
        ;


    py::class_<SpaceConfiguration>(
        module,
        "SpaceConfiguration",
        R"(
    TODO
)")

        .def(
            py::init<Mobility, SpaceDomainItemType>())

        ;

    py::class_<SpaceDomain, hdf5::Group>(
        module,
        "SpaceDomain",
        R"(
    TODO
)")

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
