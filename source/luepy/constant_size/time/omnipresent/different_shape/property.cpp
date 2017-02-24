#include "lue/cxx_api/constant_size/time/omnipresent/different_shape/property.h"
#include <pybind11/numpy.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace python {
namespace constant_size {
namespace time {
namespace omnipresent {
namespace different_shape {

void init_property_class(
    py::module& module)
{

    py::class_<
        lue::constant_size::time::omnipresent::different_shape::Property,
        lue::constant_size::time::omnipresent::Property>(
            module,
            "Property",
            "Property docstring...")

        .def(
            py::init<Property& /* , hid_t const */>(),
            "__init__ docstring..."
            "group"_a,
            // "type"_a,
            py::keep_alive<1, 2>())

        // .def("reserve_items",
        //         &time::omnipresent::different_shape::
        //             Property::reserve_items,
        //     "reserve docstring...",
        //     py::return_value_policy::reference_internal)

        .def(
            "reserve_items",
            [](
                lue::constant_size::time::omnipresent::different_shape::Property&
                    property,
                py::array_t<extent_t, py::array::c_style>& shapes) ->
                    lue::constant_size::time::omnipresent::different_shape::Item& {

                static_assert(sizeof(extent_t) == sizeof(uint64_t), "");

                // shapes must be an nD array where:
                // - the number of dimensions must equal rank + 1
                // - the first dimension corresponds with the nr_items
                // - subsequent dimensions correspond with the extents of
                //   each item's value

                auto const array_info = shapes.request();

                if(static_cast<rank_t>(array_info.ndim) != 2) {
                    throw std::runtime_error(
                        "rank of shapes array (" +
                        std::to_string(array_info.ndim) + ") must equal 2");
                }

                if(array_info.shape[1] != property.values().rank()) {
                    throw std::runtime_error(
                        "extent of second dimension of shapes array (" +
                        std::to_string(array_info.shape[1]) +
                        ") must equal rank of values (" +
                        std::to_string(property.values().rank()) + ")");
                }

                count_t const nr_items = array_info.shape[0];

                return property.reserve_items(nr_items,
                    static_cast<extent_t*>(array_info.ptr));
            },
            "reserve_items docstring...",
            py::return_value_policy::reference_internal)

        .def_property_readonly(
            "values",
            (lue::constant_size::time::omnipresent::different_shape::Item const& (lue::constant_size::time::omnipresent::different_shape::Property::*)() const) &lue::constant_size::time::omnipresent::different_shape::Property::values,
            // &lue::constant_size::time::omnipresent::different_shape::Property::values,
            "values docstring...",
            py::return_value_policy::reference_internal)
    ;

}

}  // namespace different
}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace python
}  // namespace lue
