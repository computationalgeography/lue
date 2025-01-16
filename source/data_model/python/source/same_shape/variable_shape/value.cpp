#include "lue/array/same_shape/variable_shape/value.hpp"
#include "submodule.hpp"
#include "lue/py/data_model/conversion.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
    namespace data_model {
        namespace same_shape {
            namespace variable_shape {

                void init_value(py::module& module)
                {

                    py::class_<Value, ValueGroup>(
                        module,
                        "Value",
                        R"(
    Value docstring...
)")

                        .def(
                            "expand",
                            [](Value& value, Index const idx, Count const nr_arrays, py::tuple const& shape)
                            {
                                auto const shape_ = tuple_to_shape(shape);

                                return value.expand(idx, nr_arrays, shape_);
                            })

                        ;
                }

            }  // namespace variable_shape
        }  // namespace same_shape
    }  // namespace data_model
}  // namespace lue
