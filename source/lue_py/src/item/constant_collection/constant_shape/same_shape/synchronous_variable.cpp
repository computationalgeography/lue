#include "lue/item/constant_collection/constant_shape/same_shape/synchronous_variable.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace constant_collection {
namespace constant_shape {
namespace same_shape {

void init_synchronous_variable(
    py::module& module)
{

    py::class_<SynchronousVariable, lue::Array>(
        module,
        "SynchronousVariable",
        "SynchronousVariable docstring...")

        .def("reserve",
            // &SynchronousVariable::reserve,
            [](
                SynchronousVariable& self,
                hsize_t nr_time_domain_items,
                hsize_t nr_items) -> SynchronousVariable&
            {
                self.reserve(nr_time_domain_items, nr_items);
                return self;
            },
            "reserve docstring...",
            py::return_value_policy::reference_internal)

        ;

}

}  // namespace same_shape
}  // namespace constant_shape
}  // namespace constant_collection
}  // namespace lue
