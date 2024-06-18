#include "lue/framework/api/cxx/local_operation.hpp"
#include <pybind11/pybind11.h>


namespace lue::api {

    void bind_local_operations(pybind11::module& module)
    {
        module.def("add", add);

        module.def(
            "uniform",
            [](Shape<Count, 2> const& array_shape,
               Shape<Count, 2> const& partition_shape,
               Field const& min_value,
               Field const& max_value)
            { return uniform(array_shape, partition_shape, min_value, max_value); });

        module.def(
            "uniform",
            [](Shape<Count, 2> const& array_shape, Field const& min_value, Field const& max_value)
            { return uniform(array_shape, min_value, max_value); });
    }

}  // namespace lue::api
