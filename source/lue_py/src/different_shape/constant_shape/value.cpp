#include "lue/array/different_shape/constant_shape/value.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace different_shape {
namespace constant_shape {

void init_value(
    py::module& module)
{

    py::class_<Value, ValueGroup>(
        module,
        "Value",
        R"(
    Value docstring...
)")

        ;

}

}  // namespace constant_shape
}  // namespace different_shape
}  // namespace lue
