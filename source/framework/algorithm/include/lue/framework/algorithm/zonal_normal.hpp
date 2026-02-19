#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array_decl.hpp"


namespace lue {

    template<typename Element, typename Policies, typename Zone, Rank rank>
    PartitionedArray<Element, rank> zonal_normal(
        Policies const& policies, PartitionedArray<Zone, rank> const& zones);

}  // namespace lue
