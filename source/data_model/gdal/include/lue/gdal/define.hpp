#pragma once
#include <array>


namespace lue::gdal {

    /*!
        @brief      Type used by GDAL for counting things
    */
    using Count = int;

    using Offset = std::array<Count, 2>;

    /*!
        @brief      Type for raster shapes

        First size is number of rows, second size is number of columns.
    */
    using Shape = std::array<Count, 2>;


    inline auto nr_elements(Shape const& shape) -> Count
    {
        return shape[0] * shape[1];
    }


    using GeoTransform = std::array<double, 6>;

}  // namespace lue::gdal
