#include "submodule.hpp"
#include "lue/array/different_shape/constant_shape/value.hpp"
#include "lue/py/data_model/conversion.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace data_model {
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

        .def("__len__",
            &Value::nr_objects)

        .def(
            "__getitem__",
            &Value::operator[]
        )

        .def("expand",
            [](
                Value& value,
                Index const idx,
                py::tuple const& shape,
                Count const nr_locations_in_time)
            {
                auto const shape_ = tuple_to_shape(shape);

                return value.expand(idx, shape_, nr_locations_in_time);
            })

        ;

}

}  // namespace constant_shape
}  // namespace different_shape
}  // namespace data_model
}  // namespace lue
