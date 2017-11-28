#include "lue/constant_size/time/omnipresent/space_box_domain.hpp"
#include "lue/constant_size/time/omnipresent/space_point_domain.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {

void init_domain_class(
    py::module& module)
{

    py::class_<Domain, constant_size::Domain>(
        module,
        "Domain",
        "Domain docstring...")

        // .def(py::init<Domain&>(),
        //     "__init__ docstring..."
        //     "group"_a,
        //     py::keep_alive<1, 2>())

        // .def_property_readonly("configuration",
        //         &time::omnipresent::Domain::configuration,
        //     "configuration docstring...",
        //     py::return_value_policy::reference_internal)

        .def_property_readonly(
            "space",
            [](
                Domain& self)
            {
                py::object object = py::none{};

                if(space_domain_exists(self)) {
                    auto space_domain = SpaceDomain(self);
                    auto const& configuration = space_domain.configuration();

                    auto const file_datatype =
                        SpaceBoxDomain::file_datatype(space_domain);

                    switch(configuration.item_type()) {
                        case SpaceDomain::Configuration::ItemType::box: {
                            object = py::cast(
                                new SpaceBoxDomain(std::move(space_domain)));
                            break;
                        }
                        case SpaceDomain::Configuration::ItemType::point: {
                            object = py::cast(
                                new SpacePointDomain(std::move(space_domain)));
                            break;
                        }
                    }
                }

                return object;

            },
            "space docstring...")

        ;

}

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
