#pragma once
#include "lue/framework/api/cxx/export.hpp"
#include "lue/framework/api/cxx/field.hpp"


namespace lue::api {

    LUE_FAPI_EXPORT auto from_gdal(std::string const& name) -> Field;

    LUE_FAPI_EXPORT auto from_gdal(std::string const& name, Shape<Count, 2> const& partition_shape) -> Field;

    // LUE_FAPI_EXPORT auto to_gdal(Field const& field, std::string const& name) -> hpx::future<void>;

    LUE_FAPI_EXPORT auto to_gdal(Field const& field, std::string const& name, std::string const& clone_name)
        -> hpx::future<void>;

}  // namespace lue::api
