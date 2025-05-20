#pragma once
#include "lue/framework/core/index_util.hpp"
#include "lue/framework/core/shape.hpp"
#include <cmath>


namespace lue::detail {

    /*!
        @brief      Determine the linear idx of an element in a @a source_shape whose linear @a target_idx
                    in a @target_shape is passed in, given a @a count

        This function is useful during resampling, when a target shape in partitions contains more partitions
        than the source shape in partitions. In order to assign values to the target partition, the
        corresponding source partition needs to be queried. The conversion factor can be computed by dividing
        the source cell size by the target cell size.
    */
    inline auto target_idx_to_source_idx(
        Shape<Count, 2> const& target_shape,
        Index const target_idx,
        Shape<Count, 2> const& source_shape,
        Count const& count)
    {
        lue_hpx_assert(target_idx < nr_elements(target_shape));

        // Determine location of element along all dimensions
        auto [idx0, idx1] = linear_idx_to_idxs(target_idx, target_shape);

        lue_hpx_assert(idx0 < std::get<0>(target_shape));
        lue_hpx_assert(idx1 < std::get<1>(target_shape));

        // Convert partition idxs to source shape
        idx0 = static_cast<Count>(std::floor(static_cast<double>(idx0) / count));
        idx1 = static_cast<Count>(std::floor(static_cast<double>(idx1) / count));

        idx0 = std::min(idx0, std::get<0>(source_shape) - 1);
        idx1 = std::min(idx1, std::get<1>(source_shape) - 1);

        lue_hpx_assert(idx0 < std::get<0>(source_shape));
        lue_hpx_assert(idx1 < std::get<1>(source_shape));

        // Return element idxs converted to linear index
        return idxs_to_linear_idx(Shape<Index, 2>{idx0, idx1}, source_shape);
    }

}  // namespace lue::detail
