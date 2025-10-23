#pragma once
#include "lue/framework/core/define.hpp"
#include <array>


namespace lue::detail {

    // Rules to convert 1D indices received on the various
    // channels, to 2D cell indices:

    // north
    // - row = 0
    // - get col from channel
    // RowIdxConverter{0};

    // south
    // - row = extent0 - 1
    // - get col from channel
    // RowIdxConverter{extent0 - 1};

    // The row is fixed, get the column from the channels
    inline auto row_idx_converter(Index const row, Index const idx) -> std::array<Index, 2>
    {
        return std::array<Index, 2>{row, idx};
    };

    // west
    // - col = 0
    // - get row from channel
    // ColIdxConverter{0};

    // east
    // - col = extent1 - 1
    // - get row from channel
    // ColIdxConverter{extent1 - 1};

    // The col is fixed, get the row from the channels
    inline auto col_idx_converter(Index const idx, Index const col) -> std::array<Index, 2>
    {
        return std::array<Index, 2>{idx, col};
    };

    // north west
    // - row = 0
    // - col = 0
    // - get at most one 'sentinel' idx from channel
    // CornerIdxConverter{0, 0};

    // north east
    // - row = 0
    // - col = extent1 - 1
    // - get at most one 'sentinel' idx from channel
    // CornerIdxConverter{0, extent1 - 1};

    // south east
    // - row = extent0 - 1
    // - col = extent1 - 1
    // - get at most one 'sentinel' idx from channel
    // CornerIdxConverter{extent0 - 1, extent1 - 1};

    // south west
    // - row = extent0 - 1
    // - col = 0
    // - get at most one 'sentinel' idx from channel
    // CornerIdxConverter{extent0 - 1, 0};

    // The row and col are fixed, discard the value from the channel
    inline auto corner_idx_converter(Index const row, Index const col) -> std::array<Index, 2>
    {
        return std::array<Index, 2>{row, col};
    };


    class RowIdxConverter
    {

        public:

            RowIdxConverter():

                RowIdxConverter{0}

            {
            }


            RowIdxConverter(Index const row):

                _row{row}

            {
            }


            auto operator()(Index const idx) const -> std::array<Index, 2>
            {
                return row_idx_converter(_row, idx);
            }


        private:

            Index _row;
    };


    class ColIdxConverter
    {

        public:

            ColIdxConverter():

                ColIdxConverter{0}

            {
            }


            ColIdxConverter(Index const col):

                _col{col}

            {
            }


            auto operator()(Index const idx) const -> std::array<Index, 2>
            {
                return row_idx_converter(idx, _col);
            }


        private:

            Index _col;
    };


    class CornerIdxConverter
    {

        public:

            CornerIdxConverter():

                CornerIdxConverter{0, 0}

            {
            }


            CornerIdxConverter(Index const row, Index const col):

                _row{row},
                _col{col}

            {
            }


            auto operator()([[maybe_unused]] Index const idx) const -> std::array<Index, 2>
            {
                return row_idx_converter(_row, _col);
            }


        private:

            Index _row;

            Index _col;
    };

}  // namespace lue::detail
