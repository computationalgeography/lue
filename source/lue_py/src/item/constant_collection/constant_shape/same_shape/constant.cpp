#include "lue/item/constant_collection/constant_shape/same_shape/constant.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace constant_collection {
namespace constant_shape {
namespace same_shape {

void init_constant(
    py::module& module)
{

    py::class_<Constant, lue::Array>(
        module,
        "Constant",
        "Constant docstring...")

        .def("reserve",
            // &Constant::reserve,
            [](Constant& self, hsize_t const nr_items) -> Constant&
            {
                self.reserve(nr_items);
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
