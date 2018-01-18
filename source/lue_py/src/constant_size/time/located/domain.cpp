#include "lue/constant_size/time/located/domain.hpp"
#include "lue/constant_size/time/located/time_box_domain.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace constant_size {
namespace time {
namespace located {

void init_domain(
    py::module& module)
{

    py::class_<Domain, constant_size::Domain>(
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

                    switch(configuration.item_type()) {
                        case TimeDomain::Configuration::ItemType::box: {
                            object = py::cast(
                                new TimeBoxDomain(std::move(time_domain)));
                            break;
                        }
                    }
                }

                return object;

            },
            "time docstring...")


        ;

}

}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
