#include "lue/item/same_shape/constant_shape/constant_collection/synchronous_value.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace same_shape {
namespace constant_shape {
namespace constant_collection {

void init_synchronous_value(
    py::module& module)
{

    py::class_<SynchronousValue, lue::Array>(
        module,
        "SynchronousValue",
        "SynchronousValue docstring...")

        .def("reserve",
            // &SynchronousValue::reserve,
            [](
                SynchronousValue& self,
                hsize_t nr_time_domain_items,
                hsize_t nr_items) -> SynchronousValue&
            {
                self.reserve(nr_items, nr_time_domain_items);
                return self;
            },
            "reserve docstring...",
            py::return_value_policy::reference_internal)

        ;

}

}  // namespace constant_collection
}  // namespace constant_shape
}  // namespace same_shape
}  // namespace lue
