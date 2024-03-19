#pragma once
#include <string>
#include <vector>


namespace lue::gdal {

    auto compare_rasters(std::string const& dataset_name1, std::string const& dataset_name2)
        -> std::vector<std::string>;

}  // namespace lue::gdal
