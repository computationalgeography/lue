#pragma once
#include "lue/gdal/export.hpp"
#include <string>
#include <vector>


namespace lue::gdal {

    LUE_GDAL_EXPORT auto compare_rasters(std::string const& dataset_name1, std::string const& dataset_name2)
        -> std::vector<std::string>;

}  // namespace lue::gdal
