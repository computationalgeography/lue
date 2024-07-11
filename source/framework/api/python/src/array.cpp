#include "lue/framework/api/cxx/create_array.hpp"
#include <pybind11/stl.h>


using namespace pybind11::literals;


namespace lue::api {

    void bind_array(pybind11::module& module)
    {

        module.def(
            "create_array",
            [](
                Shape<Count, 2> const& array_shape,
                Field const& fill_value,
                std::optional<Shape<Count, 2>> const& partition_shape) -> Field
            {
                return partition_shape ? create_array(array_shape, *partition_shape, fill_value)
                                       : create_array(array_shape, fill_value);
            },
            "array_shape"_a,
            "fill_value"_a,
            pybind11::kw_only(),
            "partition_shape"_a = std::optional<Shape<Count, 2>>{},
            pybind11::return_value_policy::move);
    }

}  // namespace lue::api
