#include "lue/cxx_api/constant_size/property.h"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace python {
namespace constant_size {

void init_property_class(
    py::module& module)
{

    py::class_<
        lue::constant_size::Property>(
            module,
            "Property",
            "Property docstring...")

        .def_property_readonly(
            "id",
            &lue::constant_size::Property::id,
            "id docstring...",
            py::return_value_policy::reference_internal)

        .def_property_readonly(
            "name",
            &lue::constant_size::Property::name,
            "name docstring...",
            py::return_value_policy::reference_internal)

        .def(
            "link_space_discretization",
            &lue::constant_size::Property::link_space_discretization,
            "name docstring...")
    ;

}

}  // namespace constant_size
}  // namespace python
}  // namespace lue
