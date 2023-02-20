#pragma once
#include <cstddef>
#include <tuple>


namespace lue {
    namespace utility {

        class GDALBlock
        {

            public:

                GDALBlock();

                GDALBlock(
                    size_t raster_size_x, size_t raster_size_y, size_t block_size_x, size_t block_size_y);

                GDALBlock(GDALBlock const&) = default;

                GDALBlock(GDALBlock&&) = default;

                ~GDALBlock() = default;

                GDALBlock& operator=(GDALBlock const&) = default;

                GDALBlock& operator=(GDALBlock&&) = default;

                size_t block_size_x() const;

                size_t block_size_y() const;

                size_t block_size() const;

                size_t nr_blocks_x() const;

                size_t nr_blocks_y() const;

                size_t nr_blocks() const;

                std::tuple<size_t, size_t> nr_valid_cells(size_t block_index_x, size_t block_index_y) const;

            private:

                size_t _raster_size_x;

                size_t _raster_size_y;

                size_t _block_size_x;

                size_t _block_size_y;

                size_t _block_size;

                size_t _nr_blocks_x;

                size_t _nr_blocks_y;

                size_t _nr_blocks;
        };

    }  // namespace utility
}  // namespace lue
