#pragma once
#include <array>


namespace lue::gdal {

    /*!
        @brief      Type used by GDAL for counting things
    */
    using Count = int;

    /*!
        @brief      Type for raster shapes

        First size is number of rows, second size is number of columns.
    */
    using Shape = std::array<Count, 2>;

}  // namespace lue::gdal
