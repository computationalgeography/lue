#include "lue/constant_size/time/omnipresent/different_shape/value.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {
namespace different_shape {

void init_value_class(
    py::module& module)
{

    py::class_<
            Value,
            constant::different_shape::Collection>(
        module,
        "Value",
        "Value docstring...")

        ;

}

}  // namespace different_shape
}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
