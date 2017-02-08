#include "lue/cxx_api/constant_size/property_set.h"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace python {
namespace constant_size {

void init_property_set_class(
    py::module& module)
{

    py::class_<
            lue::constant_size::PropertySet>(
                module,
                "PropertySet",
                "PropertySet docstring...")

        // Group API
        .def_property_readonly(
            "id",
            &lue::constant_size::PropertySet::id,
            "id docstring...",
            py::return_value_policy::reference_internal)

        .def_property_readonly(
            "domain",
            &lue::constant_size::PropertySet::domain,
            "domain docstring...",
            py::return_value_policy::reference_internal)

        // PropertySet API
        // .def("add_property", &PropertySet::add_property,
        //     "add_property docstring...",
        //     py::return_value_policy::reference_internal)

        .def_property_readonly(
            "configuration",
            &lue::constant_size::PropertySet::configuration,
            "configuration docstring...",
            py::return_value_policy::reference_internal)

        .def_property_readonly(
            "properties",
            &lue::constant_size::PropertySet::properties,
            "properties docstring...",
            py::return_value_policy::reference_internal)
    ;

}

}  // namespace constant_size
}  // namespace python
}  // namespace lue
