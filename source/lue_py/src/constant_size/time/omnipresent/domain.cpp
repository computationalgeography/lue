#include "lue/constant_size/time/omnipresent/space_box_domain.hpp"
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
                assert(self.id().is_valid());
                assert(self.space().id().is_valid());
                auto const& space = self.space();

                py::object object;

                // TODO switch on configuration
                // auto const& configuration = space.configuration();

                assert(self.id().is_valid());
                assert(space.id().is_valid());
                auto file_datatype = SpaceBoxDomain::file_datatype(space.id());

                object = py::cast(new SpaceBoxDomain(space,
                    memory_datatype(file_datatype)));

                return object;
            },
            "space docstring...")

        ;

}

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
