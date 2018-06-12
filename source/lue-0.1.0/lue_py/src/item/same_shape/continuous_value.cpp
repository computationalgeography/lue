#include "lue/item/same_shape/continuous_value.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace same_shape {

void init_continuous_value(
    py::module& module)
{

    py::class_<ContinuousValue, lue::Array>(
        module,
        "ContinuousValue",
        "ContinuousValue docstring...")

        .def("reserve",
            // &ContinuousValue::reserve,
            [](ContinuousValue& self, hsize_t const nr_items) -> ContinuousValue&
            {
                self.reserve(nr_items);
                return self;
            },
            "reserve docstring...",
            py::return_value_policy::reference_internal)

        ;

}

}  // namespace same_shape
}  // namespace lue
