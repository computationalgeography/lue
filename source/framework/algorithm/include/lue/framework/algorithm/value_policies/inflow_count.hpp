#pragma once
#include "lue/framework/algorithm/inflow_count.hpp"
#include <concepts>


namespace lue {
    namespace policy::inflow_count {

        template<std::integral CountElement, std::integral FlowDirectionElement>
        using DefaultValuePolicies = Policies<
            AllValuesWithinDomain<FlowDirectionElement>,
            OutputsPolicies<
                OutputPolicies<DefaultOutputNoDataPolicy<CountElement>, AllValuesWithinRange<CountElement>>>,
            InputsPolicies<SpatialOperationInputPolicies<
                DetectNoDataByValue<FlowDirectionElement>,
                FlowDirectionHalo<FlowDirectionElement>>>>;

    }  // namespace policy::inflow_count


    namespace value_policies {

        template<std::integral CountElement, std::integral FlowDirectionElement>
        auto inflow_count(PartitionedArray<FlowDirectionElement, 2> const& flow_direction)
            -> PartitionedArray<CountElement, 2>
        {
            using Policies = policy::inflow_count::DefaultValuePolicies<CountElement, FlowDirectionElement>;

            return inflow_count(Policies{}, flow_direction);
        }

    }  // namespace value_policies
}  // namespace lue
