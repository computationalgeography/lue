#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array.hpp"


namespace lue {

    template<typename Policies, typename Element, Rank rank, typename Kernel>
    PartitionedArray<Element, rank> focal_majority(
        Policies const& policies, PartitionedArray<Element, rank> const& array, Kernel const& kernel);

}  // namespace lue
