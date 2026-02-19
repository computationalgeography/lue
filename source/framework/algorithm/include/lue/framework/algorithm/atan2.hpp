#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array_decl.hpp"


namespace lue {

    template<typename Policies, typename Element, Rank rank>
    auto atan2(
        Policies const& policies,
        PartitionedArray<Element, rank> const& array1,
        PartitionedArray<Element, rank> const& array2) -> PartitionedArray<Element, rank>;

}  // namespace lue
