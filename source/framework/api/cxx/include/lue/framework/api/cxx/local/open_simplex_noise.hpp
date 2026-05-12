#pragma once
#include "lue/framework/api/cxx/export.hpp"
#include "lue/framework/api/cxx/field.hpp"


namespace lue::api {

    auto LUE_FAPI_EXPORT open_simplex_noise(Field const& x_coordinates, Field const& y_coordinates, int seed)
        -> Field;

}  // namespace lue::api
