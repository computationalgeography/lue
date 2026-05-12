#pragma once
#include "lue/framework/api/cxx/export.hpp"
#include "lue/framework/api/cxx/field.hpp"


namespace lue::api {

    auto LUE_FAPI_EXPORT greater_than_equal_to(Field const& field1, Field const& field2) -> Field;

}  // namespace lue::api
