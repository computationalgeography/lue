#pragma once
#include "lue/core/define.hpp"


namespace lue::data_model {

    class IndexRange
    {

        public:

            IndexRange(Index begin, Index end);

            IndexRange(IndexRange const& other) = default;

            IndexRange(IndexRange&& other) = default;

            ~IndexRange() = default;

            auto operator=(IndexRange const& other) -> IndexRange& = default;

            auto operator=(IndexRange&& other) -> IndexRange& = default;

            auto begin() const -> Index;

            auto end() const -> Index;

            auto size() const -> hsize_t;

        private:

            Index _begin;

            Index _end;
    };


    inline IndexRange::IndexRange(Index const begin, Index const end):

        _begin{begin},
        _end{end}

    {
        assert(begin <= end);
    }


    inline auto IndexRange::begin() const -> Index
    {
        return _begin;
    }


    inline auto IndexRange::end() const -> Index
    {
        return _end;
    }


    inline auto IndexRange::size() const -> hsize_t
    {
        return _end - _begin;
    }

}  // namespace lue::data_model
