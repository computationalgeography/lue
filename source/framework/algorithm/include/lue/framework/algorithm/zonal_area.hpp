#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array_decl.hpp"


namespace lue {

    template<typename Count, typename Policies, typename Zone, Rank rank>
    PartitionedArray<Count, rank> zonal_area(
        Policies const& policies, PartitionedArray<Zone, rank> const& zones);

}  // namespace lue
