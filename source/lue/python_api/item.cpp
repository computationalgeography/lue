#include "lue/cxx_api/item.h"
#include "lue/cxx_api/array.h"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace python {

void init_item(
        py::module& module)
{

    // py::class_<time::Item>(module,
    //     "Item", // py::base<hdf5::Group>(),
    //     "Item docstring...")
    // ;

    // py::class_<time::omnipresent::Item>(module,
    //     "O_Item", py::base<time::Item>(),
    //     "O_Item docstring...")
    // ;

    py::class_<time::omnipresent::constant_shape::Item>(module,
        "O_CS_Item",
        // py::base<time::omnipresent::Item>(),
        py::base<Array>(),
        "O_CS_Item docstring...")

        // .def(py::init<Property&, hid_t const>(),
        //     "__init__ docstring..."
        //     "group"_a, "type"_a,
        //     py::keep_alive<1, 2>())

        // .def("reserve_items",
        //         &api::time::omnipresent::constant_shape::
        //             Property::reserve_items,
        //     "reserve docstring...",
        //     py::return_value_policy::reference_internal)

        // .def_property_readonly("values",
        //         &api::time::omnipresent::constant_shape::Property::values,
        //     "values docstring...",
        //     py::return_value_policy::reference_internal)
    ;

}

}  // namespace python
}  // namespace lue
