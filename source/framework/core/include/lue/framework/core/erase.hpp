#pragma once
#include "lue/framework/core/assert.hpp"
#include "lue/framework/core/define.hpp"
#include "lue/framework/core/shape.hpp"
#include <algorithm>


namespace lue {

template<
    typename Element,
    Rank rank,
    template<typename> typename Elements>
Shape<Index, rank> erase(
    Elements<Element>& elements,
    Shape<Index, rank> shape,
    Rank const dimension_idx,
    Index const hyperslab_begin_idx,
    Index const hyperslab_end_idx)
{
    lue_hpx_assert(dimension_idx < rank);
    lue_hpx_assert(std::size(shape) == rank);
    lue_hpx_assert(hyperslab_begin_idx <= hyperslab_end_idx);
    lue_hpx_assert(hyperslab_end_idx <= shape[dimension_idx]);

    if(hyperslab_begin_idx == hyperslab_end_idx) {
        return shape;  // Nothing to do
    }

    // Number of elements to skip from the start of the 1D array to
    // reach elements to erase
    Count const offset{
        hyperslab_begin_idx * std::accumulate(
            shape.begin() + dimension_idx + 1, shape.end(), Count{1}, std::multiplies<Count>{})};
    lue_hpx_assert(offset >= 0);

    Count const count{
        (hyperslab_end_idx - hyperslab_begin_idx) * std::accumulate(
            shape.begin() + dimension_idx + 1, shape.end(), Count{1}, std::multiplies<Count>{})};
    lue_hpx_assert(count > 0);

    Count const stride{
        std::accumulate(shape.begin() + dimension_idx, shape.end(), Count{1}, std::multiplies<Count>{})};

    for(auto slab_begin = elements.begin() + offset; slab_begin < elements.end();
        slab_begin += (stride - count))
    {
        elements.erase(slab_begin, slab_begin + count);
    }

    shape[dimension_idx] -= hyperslab_end_idx - hyperslab_begin_idx;

    return shape;
}

}  // namespace lue
