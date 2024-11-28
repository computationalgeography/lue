#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array.hpp"


namespace lue {

    template<typename Policies, typename FlowDirectionElement, typename MaterialElement, Rank rank>
    auto accu3(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
        PartitionedArray<MaterialElement, rank> const& external_inflow)
        -> PartitionedArray<MaterialElement, rank>;

}  // namespace lue
