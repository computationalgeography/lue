#pragma once
#include <gdal_priv.h>
#include <string>


namespace lue::gdal {

    auto as_string(GDALDataType const& data_type) -> std::string;

}  // namespace lue::gdal
