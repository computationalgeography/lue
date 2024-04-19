#pragma once
#include "lue/framework/api/cxx/export.hpp"
#include "lue/framework/api/cxx/field.hpp"


namespace lue {

    auto LUE_FAPI_EXPORT add(Field const& field1, Field const& field2) -> Field;

}  // namespace lue
