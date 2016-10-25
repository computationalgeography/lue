#include "lue/cxx_api/item.h"
#include "lue/cxx_api/array.h"
#include "lue/python_api/util.h"
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


    py::class_<time::omnipresent::same_shape::Item>(module,
        "O_CS_Item",
        // py::base<time::omnipresent::Item>(),
        py::base<Array>(),
        "O_CS_Item docstring...")

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


    py::class_<time::omnipresent::different_shape::Item>(module,
        "O_VS_Item",
        // py::base<time::omnipresent::Item>(),
        // py::base<Array>(),
        "O_VS_Item docstring...")

        .def_property_readonly("dtype", [](
                    time::omnipresent::different_shape::Item const& self) {
                return hdf5_type_id_to_numpy_dtype(self.type_id());
            },
            "dtype docstring..."
        )

        .def_property_readonly("rank",
                &time::omnipresent::different_shape::Item::rank,
            "rank docstring..."
        )

        .def("__len__",
            &time::omnipresent::different_shape::Item::nr_items
        )

        .def("__getitem__", [](
                time::omnipresent::different_shape::Item const& values,
                index_t const idx) {
            if(idx >= values.nr_items()) {
                throw py::index_error();
            }

            return values[idx];
        })

    ;

}

}  // namespace python
}  // namespace lue
