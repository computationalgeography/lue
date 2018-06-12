#include "lue/domain.hpp"
#include "lue/space_domain.hpp"
#include "lue/time_domain.hpp"
#include "lue/constant_collection/time/omnipresent/space_box_domain.hpp"
#include "lue/constant_collection/time/omnipresent/space_point_domain.hpp"
#include "lue/constant_collection/time/located/time_box_domain.hpp"
// #include "lue/constant_collection/time/located/time_point_domain.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {

void init_domain_class(
    py::module& module)
{

    py::class_<Domain, hdf5::Group>(
        module,
        "Domain",
        "Domain docstring...")

        .def_property_readonly(
            "time",
            [](
                Domain& self)
            {
                py::object object = py::none{};

                if(time_domain_exists(self)) {
                    auto time_domain = TimeDomain(self);
                    auto const& configuration = time_domain.configuration();

                    switch(configuration.type<TimeDomainItemType>()) {
                        case TimeDomainItemType::box: {
                            object = py::cast(
                                new constant_collection::time::located::TimeBoxDomain(
                                    constant_collection::time::located::TimeDomain{self}));
                                    // std::move(time_domain)));
                            break;
                        }
                        // case TimeDomainItemType::point: {
                        //     object = py::cast(
                        //         new constant_collection::time::located::TimePointDomain(
                        //             std::move(time_domain)));
                        //     break;
                        // }
                    }
                }

                return object;

            },
            "time docstring...")

        .def_property_readonly(
            "space",
            [](
                Domain& self)
            {
                py::object object = py::none{};

                if(space_domain_exists(self)) {

                    auto space_domain = SpaceDomain(self);
                    auto const& configuration = space_domain.configuration();

                    switch(configuration.type<Mobility>()) {
                        case Mobility::stationary: {
                            switch(configuration.type<SpaceDomainItemType>()) {
                                case SpaceDomainItemType::box: {
                                    object = py::cast(
                                        new constant_collection::time::omnipresent::SpaceBoxDomain(
                                            constant_collection::SpaceDomain{self}));
                                            // std::move(space_domain)));
                                    break;
                                }
                                case SpaceDomainItemType::point: {
                                    object = py::cast(
                                        new constant_collection::time::omnipresent::SpacePointDomain(
                                            constant_collection::SpaceDomain{self}));
                                            // std::move(space_domain)));
                                    break;
                                }
                            }
                        }
                    }
                }

                return object;

            },
            "space docstring...")

        ;

}

}  // namespace lue
