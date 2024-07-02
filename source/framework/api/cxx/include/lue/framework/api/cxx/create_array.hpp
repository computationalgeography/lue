#pragma once
#include "lue/framework/api/cxx/array.hpp"
#include "lue/framework/api/cxx/export.hpp"
#include "lue/framework/api/cxx/scalar.hpp"


namespace lue::api {

    auto LUE_FAPI_EXPORT create_array(
        Shape<Count, 2> const& array_shape, Shape<Count, 2> const& partition_shape, Scalar const& fill_value)
        -> Array;

    auto LUE_FAPI_EXPORT create_array(Shape<Count, 2> const& array_shape, Scalar const& fill_value) -> Array;

}  // namespace lue::api
