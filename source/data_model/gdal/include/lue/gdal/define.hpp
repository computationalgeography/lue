#pragma once
#include <array>


namespace lue::gdal {

    /*!
        @brief      Type used by GDAL for counting things
    */
    using Count = int;

    /*!
        @brief      Type for offsets

        First coordinate is the y-coordinate, second coordinate is the x-coordinate. The
        coordinates are relative to the upper left corner of the raster band. In case of a cell
        offset, the upper left cell has offset (0, 0).
    */
    using Offset = std::array<Count, 2>;

    /*!
        @brief      Type for raster shapes

        First size is number of rows, second size is number of columns.
    */
    using Shape = std::array<Count, 2>;


    /*!
        @brief      Return the number of elements in a shape of a raster (for example)
    */
    inline auto nr_elements(Shape const& shape) -> Count
    {
        return shape[0] * shape[1];
    }


    /*!
        @brief      Type used by GDAL for no-data values
    */
    using NoDataValue = double;


    /*!
        @brief      Type used by GDAL for coordinates
    */
    using Coordinate = double;


    /*!
        @brief      Type used by GDAL for extents, e.g. for cell width, cell height
    */
    using Extent = double;


    /*!
        @brief      Type for representing transformation information
        @sa         gdal::geo_transform
    */
    using GeoTransform = std::array<Coordinate, 6>;

}  // namespace lue::gdal
