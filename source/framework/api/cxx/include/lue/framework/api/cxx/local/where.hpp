#pragma once
#include "lue/framework/api/cxx/export.hpp"
#include "lue/framework/api/cxx/field.hpp"


namespace lue::api {

    auto LUE_FAPI_EXPORT where(Field const& field1, Field const& field2) -> Field;

    auto LUE_FAPI_EXPORT where(Field const& field1, Field const& field2, Field const& field3) -> Field;

}  // namespace lue::api
