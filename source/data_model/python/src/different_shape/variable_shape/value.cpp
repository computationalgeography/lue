#include "submodule.hpp"
#include "lue/array/different_shape/variable_shape/value.hpp"
#include "lue/py/conversion.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace data_model {
namespace different_shape {
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

        .def(
            "__getitem__",
            &Value::operator[]
        )

        .def("expand",
            [](
                Value& value,
                Index const idx,
                ID const& id,
                py::tuple const& shape)
            {
                auto const shape_ = tuple_to_shape(shape);

                return value.expand(idx, 1, &id, &shape_);
            })

        ;

}

}  // namespace variable_shape
}  // namespace different_shape
}  // namespace data_model
}  // namespace lue
