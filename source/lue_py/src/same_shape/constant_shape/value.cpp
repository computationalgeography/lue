#include "lue/array/same_shape/constant_shape/value.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace same_shape {
namespace constant_shape {

void init_value(
    py::module& module)
{

    py::class_<Value, Array>(
        module,
        "Value",
        R"(
    Value docstring...
)")

        ;

}

}  // namespace constant_shape
}  // namespace same_shape
}  // namespace lue
