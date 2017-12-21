#include "lue/item/constant_size/constant/same_shape/collection.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace constant_size {
namespace constant {
namespace same_shape {

void init_collection(
    py::module& module)
{

    py::class_<Collection, lue::Array>(
        module,
        "Collection",
        "Collection docstring...")

        .def("reserve",
            // &Collection::reserve,
            [](Collection& self, hsize_t const nr_items) -> Collection&
            {
                self.reserve(nr_items);
                return self;
            },
            "reserve docstring...",
            py::return_value_policy::reference_internal)

        ;

}

}  // namespace same_shape
}  // namespace constant
}  // namespace constant_size
}  // namespace lue
