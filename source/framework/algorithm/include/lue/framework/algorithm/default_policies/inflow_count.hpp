#pragma once
#include "lue/framework/algorithm/inflow_count.hpp"
#include <concepts>


namespace lue {
    namespace policy::inflow_count {

        template<std::integral CountElement, std::integral FlowDirectionElement>
        using DefaultPolicies = Policies<
            AllValuesWithinDomain<FlowDirectionElement>,
            OutputsPolicies<OutputPolicies<DontMarkNoData<CountElement>, AllValuesWithinRange<CountElement>>>,
            InputsPolicies<SpatialOperationInputPolicies<
                SkipNoData<FlowDirectionElement>,
                FlowDirectionHalo<FlowDirectionElement>>>>;

    }  // namespace policy::inflow_count


    namespace default_policies {

        template<std::integral CountElement, std::integral FlowDirectionElement>
        auto inflow_count(PartitionedArray<FlowDirectionElement, 2> const& flow_direction)
            -> PartitionedArray<CountElement, 2>
        {
            using Policies = policy::inflow_count::DefaultPolicies<CountElement, FlowDirectionElement>;

            return inflow_count(Policies{}, flow_direction);
        }

    }  // namespace default_policies
}  // namespace lue
