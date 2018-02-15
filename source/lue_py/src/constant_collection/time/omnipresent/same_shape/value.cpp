#include "lue/constant_collection/time/omnipresent/same_shape/value.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace constant_collection {
namespace time {
namespace omnipresent {
namespace same_shape {

void init_value_class(
    py::module& module)
{

    py::class_<
            Value,
            lue::constant_shape::same_shape::ContinuousValue>(
        module,
        "Value",
        "Value docstring...")

    ;

}

}  // namespace same_shape
}  // namespace omnipresent
}  // namespace time
}  // namespace constant_collection
}  // namespace lue
