#include "lue/item/constant_size/constant/same_shape.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace constant_size {
namespace constant {

void init_same_shape(
    py::module& module)
{

    py::class_<SameShape, lue::Array>(
        module,
        "SameShape",
        "SameShape docstring...")

        .def("reserve",
            &SameShape::reserve,
            "reserve docstring...",
            py::return_value_policy::reference_internal)

        ;

}

}  // namespace constant
}  // namespace constant_size
}  // namespace lue
