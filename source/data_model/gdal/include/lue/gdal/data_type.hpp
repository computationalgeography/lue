#pragma once
#include "lue/gdal/export.hpp"
#include <gdal_priv.h>
#include <string>


namespace lue::gdal {

    LUE_GDAL_EXPORT auto as_string(GDALDataType const& data_type) -> std::string;

}  // namespace lue::gdal
