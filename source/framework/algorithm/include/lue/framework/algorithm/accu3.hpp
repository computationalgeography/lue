#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array.hpp"


namespace lue {

    template<typename Policies, typename FlowDirectionElement, typename MaterialElement, Rank rank>
    PartitionedArray<MaterialElement, rank> accu3(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
        PartitionedArray<MaterialElement, rank> const& external_inflow);

}  // namespace lue
