#pragma once
#include "lue/framework/api/cxx/export.hpp"
#include "lue/framework/api/cxx/field.hpp"


namespace lue::api {

    auto LUE_FAPI_EXPORT accu(FlowDirectionField const& flow_direction, Field const& inflow) -> Field;

}  // namespace lue::api
