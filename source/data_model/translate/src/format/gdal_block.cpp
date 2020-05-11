#include "lue/translate/format/gdal_block.hpp"
#include <cassert>


namespace lue {
namespace utility {


GDALBlock::GDALBlock()

    : GDALBlock{0, 0, 0, 0}

{
}


GDALBlock::GDALBlock(
    size_t const raster_size_x,
    size_t const raster_size_y,
    size_t const block_size_x,
    size_t const block_size_y)

    : _raster_size_x{raster_size_x},
      _raster_size_y{raster_size_y},
      _block_size_x{block_size_x},
      _block_size_y{block_size_y},
      _block_size{0},
      _nr_blocks_x{0},
      _nr_blocks_y{0},
      _nr_blocks{0}

{
    assert(_block_size_x <= _raster_size_x);
    assert(_block_size_y <= _raster_size_y);

    _block_size = _block_size_x * _block_size_y;

    if(_block_size_x > 0) {
        _nr_blocks_x = (_raster_size_x + _block_size_x - 1) / _block_size_x;
    }

    if(_block_size_y > 0) {
        _nr_blocks_y = (_raster_size_y + _block_size_y - 1) / _block_size_y;
    }

    _nr_blocks = _nr_blocks_x * _nr_blocks_y;
}


size_t GDALBlock::block_size_x() const
{
    return _block_size_x;
}


size_t GDALBlock::block_size_y() const
{
    return _block_size_y;
}


size_t GDALBlock::block_size() const
{
    return _block_size;
}


size_t GDALBlock::nr_blocks_x() const
{
    return _nr_blocks_x;
}


size_t GDALBlock::nr_blocks_y() const
{
    return _nr_blocks_y;
}


size_t GDALBlock::nr_blocks() const
{
    return _nr_blocks;
}


std::tuple<size_t, size_t> GDALBlock::nr_valid_cells(
    size_t const block_index_x,
    size_t const block_index_y) const
{
    size_t nr_valid_cells_x{};
    size_t nr_valid_cells_y{};

    // Compute the portion of the block that is valid
    // for partial edge blocks.
    if((block_index_x + 1) * _block_size_x > _raster_size_x) {
        nr_valid_cells_x = _raster_size_x - block_index_x * _block_size_x;
    }
    else {
        nr_valid_cells_x = _block_size_x;
    }

    if((block_index_y +1 ) * _block_size_y > _raster_size_y) {
        nr_valid_cells_y = _raster_size_y - block_index_y * _block_size_y;
    }
    else {
        nr_valid_cells_y = _block_size_y;
    }

    return std::make_tuple(nr_valid_cells_x, nr_valid_cells_y);
}

}  // namespace utility
}  // namespace lue
