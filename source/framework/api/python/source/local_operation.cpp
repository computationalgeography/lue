#include "lue/framework/api/cxx/local_operation.hpp"
#include <pybind11/stl.h>


namespace lue::api {

    void bind_local_operations(pybind11::module& module)
    {
        module.def("add", add);
        module.def("close_to", close_to);
        module.def("equal_to", equal_to);
        module.def("greater_than", greater_than);
        module.def("greater_than_equal_to", greater_than_equal_to);
        module.def("less_than", less_than);
        module.def("logical_and", logical_and);
        module.def("logical_exclusive_or", logical_exclusive_or);
        module.def("logical_inclusive_or", logical_inclusive_or);
        module.def("logical_not", logical_not);

        module.def(
            "uniform",
            [](Shape<Count, 2> const& array_shape,
               Shape<Count, 2> const& partition_shape,
               Field const& min_value,
               Field const& max_value) -> Field
            { return uniform(array_shape, partition_shape, min_value, max_value); });

        module.def(
            "uniform",
            [](Shape<Count, 2> const& array_shape, Field const& min_value, Field const& max_value) -> Field
            { return uniform(array_shape, min_value, max_value); });
    }

}  // namespace lue::api
