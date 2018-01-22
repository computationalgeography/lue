#include "lue/constant_size/time/located/constant_shape/same_shape/value.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace constant_size {
namespace time {
namespace located {
namespace constant_shape {
namespace same_shape {


void init_value(
    py::module& module)
{

    py::class_<
            Value,
            variable::constant_shape::same_shape::Collection>(
        module,
        "Value",
        "Value docstring...")

        // .def("reserve",
        //     &Value::reserve,
        //     "reserve docstring...",
        //     py::return_value_policy::reference_internal)

        // .def_property_readonly("values",
        //         &api::time::omnipresent::same_shape::Property::values,
        //     "values docstring...",
        //     py::return_value_policy::reference_internal)
    ;

}

}  // namespace same_shape
}  // namespace constant_shape
}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
