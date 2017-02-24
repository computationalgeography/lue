#include "lue/cxx_api/constant_size/time/omnipresent/same_shape/property.h"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace python {
namespace constant_size {
namespace time {
namespace omnipresent {
namespace same_shape {

void init_property_class(
    py::module& module)
{

    py::class_<
            lue::constant_size::time::omnipresent::same_shape::Property,
            lue::constant_size::time::omnipresent::Property>(
                module,
                "Property",
                "Property docstring...")

        .def(
            py::init<Property&, hid_t const>(),
            "__init__ docstring..."
            "group"_a,
            "type"_a,
            py::keep_alive<1, 2>())

        .def(
            "reserve_items",
            &lue::constant_size::time::omnipresent::same_shape::
                Property::reserve_items,
            "reserve docstring...",
            py::return_value_policy::reference_internal)

        .def_property_readonly(
            "values",
            (lue::constant_size::time::omnipresent::same_shape::Item const& (lue::constant_size::time::omnipresent::same_shape::Property::*)() const) &lue::constant_size::time::omnipresent::same_shape::Property::values,
            // &lue::constant_size::time::omnipresent::same_shape::Property::values,
            "values docstring...",
            py::return_value_policy::reference_internal)
    ;

}

}  // namespace same_shape
}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace python
}  // namespace lue
