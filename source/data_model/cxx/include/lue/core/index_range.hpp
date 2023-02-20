#pragma once
#include "lue/core/define.hpp"


namespace lue {
    namespace data_model {

        class IndexRange
        {

            public:

                IndexRange(Index begin, Index end);

                IndexRange(IndexRange const&) = default;

                IndexRange(IndexRange&&) = default;

                ~IndexRange() = default;

                IndexRange& operator=(IndexRange const&) = default;

                IndexRange& operator=(IndexRange&&) = default;

                Index begin() const;

                Index end() const;

                hsize_t size() const;

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


        inline Index IndexRange::begin() const
        {
            return _begin;
        }


        inline Index IndexRange::end() const
        {
            return _end;
        }


        inline hsize_t IndexRange::size() const
        {
            return _end - _begin;
        }

    }  // namespace data_model
}  // namespace lue
