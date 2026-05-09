#include "lue/framework/api/cxx/local_operation.hpp"
#include <pybind11/stl.h>


namespace lue::api {

    void bind_local_operations(pybind11::module& module)
    {
        module.def("acos", acos);
        module.def("add", add);
        module.def("asin", asin);
        module.def("atan", atan);
        module.def("atan2", atan2);
        module.def("close_to", close_to);
        module.def("cos", cos);
        module.def("equal_to", equal_to);
        module.def("greater_than", greater_than);
        module.def("greater_than_equal_to", greater_than_equal_to);
        module.def("less_than", less_than);
        module.def("less_than_equal_to", less_than_equal_to);
        module.def("logical_and", logical_and);
        module.def("logical_exclusive_or", logical_exclusive_or);
        module.def("logical_inclusive_or", logical_inclusive_or);
        module.def("logical_not", logical_not);
        module.def("not_equal_to", equal_to);
        module.def("sin", sin);
        module.def("tan", tan);
        module.def(
            "where",
            [](Field const& condition_field, Field const& true_field) -> Field
            { return where(condition_field, true_field); });
        module.def(
            "where",
            [](Field const& condition_field, Field const& true_field, Field const& false_field) -> Field
            { return where(condition_field, true_field, false_field); });

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
