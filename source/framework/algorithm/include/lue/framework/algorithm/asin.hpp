#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array.hpp"


namespace lue {

    template<typename Policies, typename Element, Rank rank>
    PartitionedArray<Element, rank> asin(
        Policies const& policies, PartitionedArray<Element, rank> const& array);

}  // namespace lue
