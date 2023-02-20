#pragma once
#include "lue/framework/core/assert.hpp"
#include "lue/framework/core/indices.hpp"
#include "lue/framework/core/shape.hpp"
#include <algorithm>


namespace lue {

    template<typename Index, typename Count, Rank rank>
    Indices<Index, rank> linear_idx_to_idxs(Index const linear_idx, Shape<Count, rank> const& shape)
    {
        lue_hpx_assert(nr_elements(shape) > 0);
        lue_hpx_assert(linear_idx < nr_elements(shape));

        Indices<Index, rank> idxs;

        for (Rank d = 0; d < rank; ++d)
        {
            idxs[d] =
                ((linear_idx /
                  std::accumulate(shape.begin() + d + 1, shape.end(), Count{1}, std::multiplies<Count>())) %
                 shape[d]);
        }

        return idxs;
    }


    template<typename Index, typename Count, Rank rank>
    Index idxs_to_linear_idx(Indices<Index, rank> idxs, Shape<Count, rank> const& shape)
    {
        lue_hpx_assert(nr_elements(shape) > 0);

        Index idx{0};

        for (Rank d = 0; d < rank; ++d)
        {
            lue_hpx_assert(idxs[d] < shape[d]);

            idx += idxs[d] *
                   std::accumulate(shape.begin() + d + 1, shape.end(), Count{1}, std::multiplies<Count>());
        }

        return idx;
    }

}  // namespace lue
