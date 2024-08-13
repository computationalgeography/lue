#pragma once
#include "lue/framework/api/cxx/export.hpp"
#include "lue/framework/api/cxx/field.hpp"
#include "lue/framework/api/cxx/literal.hpp"


namespace lue::api {

    LUE_FAPI_EXPORT auto create_scalar(Literal const& value) -> Field;

}  // namespace lue::api
