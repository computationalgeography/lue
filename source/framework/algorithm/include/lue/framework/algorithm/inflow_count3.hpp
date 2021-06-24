#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array.hpp"


namespace lue {
    namespace policy::inflow_count3 {

        template<
            typename CountElement,
            typename FlowDirectionElement>
        using DefaultPolicies = Policies<
            AllValuesWithinDomain<FlowDirectionElement>,
            OutputsPolicies<
                OutputPolicies<DontMarkNoData<CountElement>>>,
            InputsPolicies<
                SpatialOperationInputPolicies<
                    SkipNoData<FlowDirectionElement>,
                    FlowDirectionHalo<FlowDirectionElement>>>>;

        template<
            typename CountElement,
            typename FlowDirectionElement>
        using DefaultValuePolicies = Policies<
            AllValuesWithinDomain<FlowDirectionElement>,
            OutputsPolicies<
                OutputPolicies<DefaultOutputNoDataPolicy<CountElement>>>,
            InputsPolicies<
                SpatialOperationInputPolicies<
                    DetectNoDataByValue<FlowDirectionElement>,
                    FlowDirectionHalo<FlowDirectionElement>>>>;

    }  // namespace policy::inflow_count3


    template<
        typename CountElement,
        typename Policies,
        typename FlowDirectionElement,
        Rank rank>
    PartitionedArray<CountElement, rank> inflow_count3(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, rank> const& flow_direction);


    template<
        typename CountElement,
        typename FlowDirectionElement,
        Rank rank>
    PartitionedArray<CountElement, rank> inflow_count3(
        PartitionedArray<FlowDirectionElement, rank> const& flow_direction)
    {
        using Policies = policy::inflow_count3::DefaultPolicies<CountElement, FlowDirectionElement>;

        return inflow_count3<CountElement>(Policies{}, flow_direction);
    }

}  // namespace lue
