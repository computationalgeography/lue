#pragma once
#include "lue/framework/api/cxx/export.hpp"
#include "lue/framework/api/cxx/field.hpp"


namespace lue {

    auto LUE_FAPI_EXPORT create_array(
        Shape<Count, 2> const& array_shape, Shape<Count, 2> const& partition_shape, Field const& fill_value)
        -> Field;

    auto LUE_FAPI_EXPORT create_array(Shape<Count, 2> const& array_shape, Field const& fill_value) -> Field;

}  // namespace lue
