#pragma once
#include "lue/framework/api/cxx/export.hpp"
#include "lue/framework/api/cxx/field.hpp"
#include "lue/framework/api/cxx/lookup_table.hpp"


namespace lue::api {

    LUE_FAPI_EXPORT auto reclassify(Field const& field, LookupTable const& lookup_table) -> Field;

}  // namespace lue::api
