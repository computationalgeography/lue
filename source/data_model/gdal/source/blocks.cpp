#include "lue/gdal/blocks.hpp"
#include <cassert>


namespace lue::gdal {
    namespace {

        auto extent_in_blocks(Count const full_extent, Count const block_extent)
        {
            assert(block_extent > 0);

            return (full_extent + block_extent - 1) / block_extent;
        }


        auto nr_valid_cells(Count const full_extent, Count const block_extent, Count const block_offset)
            -> Count
        {
            return ((block_offset + 1) * block_extent > full_extent)
                       ? full_extent - (block_offset * block_extent)
                       : block_extent;
        }

    }  // Anonymous namespace


    /*!
        @brief      Construct an instance based on raster shape and a block shape
        @param      raster_shape Shape of the raster
        @param      block_shape Shape of the block. The extent of each dimension must be equal
                    or smaller to the corresponding extent of the raster.
    */
    Blocks::Blocks(Shape const& raster_shape, Shape const& block_shape):

        _raster_shape{raster_shape},
        _block_shape{block_shape},
        _shape_in_blocks{0, 0}

    {
        assert(_block_shape[0] <= _raster_shape[0]);
        assert(_block_shape[1] <= _raster_shape[1]);

        if (_block_shape[0] > 0)
        {
            _shape_in_blocks[0] = extent_in_blocks(_raster_shape[0], _block_shape[0]);
        }

        if (_block_shape[1] > 0)
        {
            _shape_in_blocks[1] = extent_in_blocks(_raster_shape[1], _block_shape[1]);
        }
    }


    /*!
        @brief      Return the raster shape
    */
    auto Blocks::raster_shape() const -> Shape
    {
        return _raster_shape;
    }


    /*!
        @brief      Return the block shape
    */
    auto Blocks::block_shape() const -> Shape
    {
        return _block_shape;
    }


    /*!
        @brief      Return the shape of the raster in blocks
    */
    auto Blocks::shape_in_blocks() const -> Shape
    {
        return _shape_in_blocks;
    }


    /*!
        @brief      Return the shape (in number of cells) of the valid cells that fall within
                    a block
        @param      block_offset Indices of the block. For the upper left block this is
                    `(0, 0)`, and for the lower right block this is
                    `(shape_in_blocks[0] - 1, shape_in_blocks[1] - 1)`.

        Valid cells are cells located within the bounds of the raster.
    */
    auto Blocks::shape_in_valid_cells(Offset const& block_offset) const -> Shape
    {
        return {
            nr_valid_cells(_raster_shape[0], _block_shape[0], block_offset[0]),
            nr_valid_cells(_raster_shape[1], _block_shape[1], block_offset[1])};
    }

}  // namespace lue::gdal
