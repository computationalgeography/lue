#pragma once
#include "lue/framework/algorithm/flow_accumulation.hpp"
#include "lue/framework/algorithm/policy/all_values_within_domain.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"
#include "lue/framework/algorithm/policy/default_value_policies.hpp"
#include "lue/framework/partitioned_array.hpp"


namespace lue {
    namespace policy::downstream {

        template<
            typename FlowDirectionElement,
            typename MaterialElement>
        using DefaultPolicies =
            flow_accumulation::DefaultPolicies<FlowDirectionElement, MaterialElement>;

        template<
            typename FlowDirectionElement,
            typename MaterialElement>
        using DefaultValuePolicies =
            flow_accumulation::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

    }  // namespace downstream::policy


    template<
        typename Policies,
        typename FlowDirectionElement,
        typename MaterialElement,
        Rank rank>
    PartitionedArray<MaterialElement, rank> downstream(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
        PartitionedArray<MaterialElement, rank> const& material);


    template<
        typename FlowDirectionElement,
        typename MaterialElement,
        Rank rank>
    PartitionedArray<MaterialElement, rank> downstream(
        PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
        PartitionedArray<MaterialElement, rank> const& material)
    {
        using Policies = policy::downstream::DefaultPolicies<FlowDirectionElement, MaterialElement>;

        return downstream(Policies{}, flow_direction, material);
    }

}  // namespace lue
