#include "lue/cxx_api/constant_size/time/omnipresent/same_shape/item.h"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace python {
namespace constant_size {
namespace time {
namespace omnipresent {
namespace same_shape {

void init_item_class(
    py::module& module)
{

    py::class_<
            lue::constant_size::time::omnipresent::same_shape::Item,
            Array>(
                module,
                "Item",
                "Item docstring...")

        .def_property_readonly(
            "shape_per_item_type",
            [](
                lue::constant_size::time::omnipresent::same_shape::Item& /* self */)
            {
                return lue::ShapePerItem::same;
            },
            "shape_per_item_type docstring...",
            py::return_value_policy::reference_internal)

        // .def(py::init<Property&, hid_t const>(),
        //     "__init__ docstring..."
        //     "group"_a, "type"_a,
        //     py::keep_alive<1, 2>())

        // .def("reserve_items",
        //         &api::time::omnipresent::same_shape::Item::reserve_items,
        //     "reserve_items docstring...",
        //     py::return_value_policy::reference_internal)

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
}  // namespace python
}  // namespace lue
