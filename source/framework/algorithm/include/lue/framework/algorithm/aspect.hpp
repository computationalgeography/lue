#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array_decl.hpp"


namespace lue {

    template<typename Policies, typename Element, Rank rank>
    PartitionedArray<Element, rank> aspect(
        Policies const& policies, PartitionedArray<Element, rank> const& elevation);

}  // namespace lue
