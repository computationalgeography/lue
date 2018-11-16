#include "submodule.hpp"
#include "lue/array/different_shape/value.hpp"
#include <pybind11/numpy.h>
#include <fmt/format.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace different_shape {

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
                py::array_t<ID> const& ids,
                py::array_t<hdf5::Shape::value_type> const& shapes) -> Value&
            {
                if(ids.ndim() != 1) {
                    throw std::runtime_error(
                        "IDs must be represented by a scalar value"
                    );
                }

                if(shapes.ndim() != 2) {
                    throw std::runtime_error(
                        "Shapes must be represented by a 1D array"
                    );
                }

                if(shapes.shape()[1] != value.rank()) {
                    throw std::runtime_error(fmt::format(
                        "Shape sizes ({}) must be equal to the rank of each "
                        "object array ({})",
                        shapes.ndim(), value.rank())
                    );
                }

                if(ids.shape()[0] != shapes.shape()[0]) {
                    throw std::runtime_error(
                        "Number of IDs must equal number of shapes");
                }

                auto const nr_objects = ids.shape()[0];

                value.expand(nr_objects, ids.data(), shapes.data());

                return value;
            })

        ;

}

}  // namespace different_shape
}  // namespace lue
