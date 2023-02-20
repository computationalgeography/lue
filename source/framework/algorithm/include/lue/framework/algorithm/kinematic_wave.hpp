#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array.hpp"


namespace lue {

    template<typename Policies, typename FlowDirectionElement, typename Element, Rank rank>
    PartitionedArray<Element, rank> kinematic_wave(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
        PartitionedArray<Element, rank> const& discharge,
        PartitionedArray<Element, rank> const& inflow,
        Element const alpha,
        Element const beta,
        Element const time_step_duration,
        PartitionedArray<Element, rank> const& channel_length);

}  // namespace lue
