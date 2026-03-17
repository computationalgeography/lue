#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array_decl.hpp"


namespace lue {

    template<typename Count, typename Policies, typename Element, typename Zone, Rank rank>
    PartitionedArray<Count, rank> zonal_diversity(
        Policies const& policies,
        PartitionedArray<Element, rank> const& array,
        PartitionedArray<Zone, rank> const& zones);

}  // namespace lue
