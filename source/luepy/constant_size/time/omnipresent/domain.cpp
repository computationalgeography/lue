#include "lue/cxx_api/constant_size/time/omnipresent/domain.h"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace python {
namespace constant_size {
namespace time {
namespace omnipresent {

void init_domain_class(
    py::module& module)
{

    py::class_<
            lue::constant_size::time::omnipresent::Domain,
            lue::constant_size::Domain>(
                module,
                "Domain",
                "Domain docstring...")

        .def(py::init<Domain&>(),
            "__init__ docstring..."
            "group"_a,
            py::keep_alive<1, 2>())

        // .def_property_readonly("configuration",
        //         &time::omnipresent::Domain::configuration,
        //     "configuration docstring...",
        //     py::return_value_policy::reference_internal)

    ;

}

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace python
}  // namespace lue
