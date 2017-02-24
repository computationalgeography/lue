#include "lue/constant_size/time/omnipresent/same_shape/value.h"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {
namespace same_shape {


void init_value_class(
    py::module& module)
{

    py::class_<
                Value,
                Array>(
            module,
            "Value",
            "Value docstring...")

        // .def_property_readonly(
        //     "shape_per_item_type",
        //     [](
        //         lue::constant_size::time::omnipresent::same_shape::Item& /* self */)
        //     {
        //         return lue::ShapePerItem::same;
        //     },
        //     "shape_per_item_type docstring...",
        //     py::return_value_policy::reference_internal)

        // .def(py::init<Property&, hid_t const>(),
        //     "__init__ docstring..."
        //     "group"_a, "type"_a,
        //     py::keep_alive<1, 2>())

        .def("reserve_values",
            &Value::reserve_values,
            "reserve_values docstring...",
            py::return_value_policy::reference_internal)

        // .def_property_readonly("values",
        //         &api::time::omnipresent::same_shape::Property::values,
        //     "values docstring...",
        //     py::return_value_policy::reference_internal)
    ;

}

}  // namespace same_shape
}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
