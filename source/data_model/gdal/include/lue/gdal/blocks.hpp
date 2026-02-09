#pragma once
#include "lue/gdal/define.hpp"
#include "lue/gdal/export.hpp"


namespace lue::gdal {

    /*!
        @brief      Utility class for handling the shape of "natural" blocks

        A natural block is a block that can be read / written efficiently from / to a raster band.
    */
    class LUE_GDAL_EXPORT Blocks
    {

        public:

            Blocks(Shape const& raster_shape, Shape const& block_shape);

            auto raster_shape() const -> Shape;

            auto block_shape() const -> Shape;

            auto shape_in_blocks() const -> Shape;

            auto shape_in_valid_cells(Offset const& block_offset) const -> Shape;

        private:

            //! Raster shape
            Shape _raster_shape;

            //! Block shape
            Shape _block_shape;

            //! Shape of the raster in block units
            Shape _shape_in_blocks;
    };

}  // namespace lue::gdal
