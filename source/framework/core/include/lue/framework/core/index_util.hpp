#pragma once
#include "lue/framework/core/assert.hpp"
#include "lue/framework/core/indices.hpp"
#include "lue/framework/core/shape.hpp"
#include <algorithm>


namespace lue {

    template<typename Index, typename Count, Rank rank>
    auto linear_idx_to_idxs(Index const linear_idx, Shape<Count, rank> const& shape) -> Indices<Index, rank>
    {
        lue_hpx_assert(nr_elements(shape) > 0);
        lue_hpx_assert(linear_idx < nr_elements(shape));

        Indices<Index, rank> idxs;

        for (Rank dimension_idx = 0; dimension_idx < rank; ++dimension_idx)
        {
            idxs[dimension_idx] =
                ((linear_idx /
                  std::accumulate(
                      shape.begin() + dimension_idx + 1, shape.end(), Count{1}, std::multiplies<Count>())) %
                 shape[dimension_idx]);
        }

        return idxs;
    }


    template<typename Index, typename Count, Rank rank>
    auto idxs_to_linear_idx(Indices<Index, rank> idxs, Shape<Count, rank> const& shape) -> Index
    {
        lue_hpx_assert(nr_elements(shape) > 0);

        Index idx{0};

        for (Rank dimension_idx = 0; dimension_idx < rank; ++dimension_idx)
        {
            lue_hpx_assert(idxs[dimension_idx] < shape[dimension_idx]);

            idx += idxs[dimension_idx] *
                   std::accumulate(
                       shape.begin() + dimension_idx + 1, shape.end(), Count{1}, std::multiplies<Count>());
        }

        return idx;
    }

}  // namespace lue
