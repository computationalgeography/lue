#include "lue/python_api/numpy.h"
#include "lue/cxx_api/constant_size/time/omnipresent/different_shape/item.h"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace python {
namespace constant_size {
namespace time {
namespace omnipresent {
namespace different_shape {

void init_item_class(
    py::module& module)
{

    py::class_<
            lue::constant_size::time::omnipresent::different_shape::Item>(
                module,
                "Item",
                "Item docstring...")

        .def_property_readonly(
            "dtype",
            [](
                lue::constant_size::time::omnipresent::different_shape::Item
                    const& self)
            {
                return hdf5_type_id_to_numpy_dtype(self.type_id());
            },
            "dtype docstring..."
        )

        .def_property_readonly(
            "rank",
            &lue::constant_size::time::omnipresent::different_shape::Item::rank,
            "rank docstring..."
        )

        .def(
            "__len__",
            &lue::constant_size::time::omnipresent::different_shape::
                Item::nr_items
        )

        .def(
            "__getitem__",
            [](
                lue::constant_size::time::omnipresent::different_shape::
                    Item const& values,
                index_t const idx)
            {
                if(idx >= values.nr_items()) {
                    throw py::index_error();
                }

                return values[idx];
            })

    ;

}

}  // namespace different_shape
}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace python
}  // namespace lue
