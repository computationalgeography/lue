#include "lue/item/constant_size/constant/same_shape.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


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
            &Collection::reserve,
            "reserve docstring...",
            py::return_value_policy::reference_internal)

        ;

}

}  // namespace same_shape
}  // namespace constant
}  // namespace constant_size
}  // namespace lue
