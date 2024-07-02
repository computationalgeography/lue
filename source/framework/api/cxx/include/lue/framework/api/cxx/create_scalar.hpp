#pragma once
#include "lue/framework/api/cxx/export.hpp"
#include "lue/framework/api/cxx/literal.hpp"
#include "lue/framework/api/cxx/scalar.hpp"


namespace lue::api {

    auto LUE_FAPI_EXPORT create_scalar(Literal const& fill_value) -> Scalar;

}  // namespace lue::api
