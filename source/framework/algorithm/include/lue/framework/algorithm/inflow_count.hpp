#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array_decl.hpp"


namespace lue {
    namespace policy::inflow_count {

        template<typename CountElement, typename FlowDirectionElement>
        using DefaultPolicies = Policies<
            AllValuesWithinDomain<FlowDirectionElement>,
            OutputsPolicies<OutputPolicies<DontMarkNoData<CountElement>, AllValuesWithinRange<CountElement>>>,
            InputsPolicies<SpatialOperationInputPolicies<
                SkipNoData<FlowDirectionElement>,
                FlowDirectionHalo<FlowDirectionElement>>>>;

        template<typename CountElement, typename FlowDirectionElement>
        using DefaultValuePolicies = Policies<
            AllValuesWithinDomain<FlowDirectionElement>,
            OutputsPolicies<
                OutputPolicies<DefaultOutputNoDataPolicy<CountElement>, AllValuesWithinRange<CountElement>>>,
            InputsPolicies<SpatialOperationInputPolicies<
                DetectNoDataByValue<FlowDirectionElement>,
                FlowDirectionHalo<FlowDirectionElement>>>>;

    }  // namespace policy::inflow_count


    template<typename CountElement, typename Policies, typename FlowDirectionElement, Rank rank>
    auto inflow_count(
        Policies const& policies, PartitionedArray<FlowDirectionElement, rank> const& flow_direction)
        -> PartitionedArray<CountElement, rank>;


    template<typename CountElement, typename FlowDirectionElement, Rank rank>
    auto inflow_count(PartitionedArray<FlowDirectionElement, rank> const& flow_direction)
        -> PartitionedArray<CountElement, rank>
    {
        using Policies = policy::inflow_count::DefaultPolicies<CountElement, FlowDirectionElement>;

        return inflow_count<CountElement>(Policies{}, flow_direction);
    }

}  // namespace lue
