#include "lue/constant_collection/time/located/constant_shape/different_shape/value.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace constant_collection {
namespace time {
namespace located {
namespace constant_shape {
namespace different_shape {

void init_value(
    py::module& module)
{

    py::class_<
            Value,
            lue::constant_shape::different_shape::constant_collection::SynchronousValue>(
        module,
        "Value",
        "Value docstring...")

        // .def_property_readonly(
        //     "dtype",
        //     [](Value const& self)
        //     {
        //         py::object object = hdf5_type_id_to_numpy_dtype(
        //             self.memory_datatype());
        //         assert(object.ptr() != nullptr);
        //         return object;
        //     },
        //     "dtype docstring..."
        // )

        // .def_property_readonly(
        //     "rank",
        //     &Value::rank,
        //     "rank docstring..."
        // )

        // .def(
        //     "__len__",
        //     &Value::nr_items,
        //     "__len__ docstring..."
        // )

        // .def(
        //     "__getitem__",
        //     [](Value const& values,
        //         size_t const idx)
        //     {
        //         if(idx >= values.nr_items()) {
        //             throw py::index_error();
        //         }

        //         return values[idx];
        //     })

        ;

}

}  // namespace different_shape
}  // namespace constant_shape
}  // namespace located
}  // namespace time
}  // namespace constant_collection
}  // namespace lue
