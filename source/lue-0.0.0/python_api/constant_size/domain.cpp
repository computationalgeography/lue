#include "lue/cxx_api/constant_size/domain.h"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace python {
namespace constant_size {

void init_domain_class(
    py::module& module)
{

    py::class_<
            lue::constant_size::Domain>(
                module,
                "Domain",
                "Domain docstring...")

        .def_property_readonly(
            "configuration",
            &lue::constant_size::Domain::configuration,
            "configuration docstring...",
            py::return_value_policy::reference_internal)

        .def_property_readonly(
            "time_domain",
            &lue::constant_size::Domain::time_domain,
            "time docstring...",
            py::return_value_policy::reference_internal)

        .def_property_readonly(
            "space_domain",
            &lue::constant_size::Domain::space_domain,
            "space docstring...",
            py::return_value_policy::reference_internal)
    ;

}

}  // namespace constant_size
}  // namespace python
}  // namespace lue
