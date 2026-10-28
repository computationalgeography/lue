#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array_decl.hpp"


namespace lue {
    namespace policy::downstream_distance {

        template<typename FlowDirectionElement, typename DistanceElement>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<FlowDirectionElement>,
            OutputElements<DistanceElement>,
            InputElements<FlowDirectionElement>>;

        template<typename FlowDirectionElement, typename DistanceElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<FlowDirectionElement>,
            OutputElements<DistanceElement>,
            InputElements<FlowDirectionElement>>;

    }  // namespace policy::downstream_distance


    template<typename Policies, typename FlowDirectionElement, typename DistanceElement, Rank rank>
    auto downstream_distance(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
        DistanceElement cell_size) -> PartitionedArray<DistanceElement, rank>;


    template<typename FlowDirectionElement, typename DistanceElement, Rank rank>
    auto downstream_distance(
        PartitionedArray<FlowDirectionElement, rank> const& flow_direction, DistanceElement const cell_size)
        -> PartitionedArray<DistanceElement, rank>
    {
        using Policies = policy::downstream_distance::DefaultPolicies<FlowDirectionElement, DistanceElement>;

        return downstream_distance(Policies{}, flow_direction, cell_size);
    }

}  // namespace lue
