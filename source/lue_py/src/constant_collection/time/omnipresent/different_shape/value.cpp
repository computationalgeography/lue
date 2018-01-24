#include "lue/constant_collection/time/omnipresent/different_shape/value.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace constant_collection {
namespace time {
namespace omnipresent {
namespace different_shape {

void init_value_class(
    py::module& module)
{

    py::class_<
            Value,
            constant_shape::different_shape::Constant>(
        module,
        "Value",
        "Value docstring...")

        ;

}

}  // namespace different_shape
}  // namespace omnipresent
}  // namespace time
}  // namespace constant_collection
}  // namespace lue
