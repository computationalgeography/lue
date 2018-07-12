#include "lue/array/same_shape/variable_shape/value.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace same_shape {
namespace variable_shape {

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

}  // namespace variable_shape
}  // namespace same_shape
}  // namespace lue
