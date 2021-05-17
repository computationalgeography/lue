#pragma once
#include "lue/framework/algorithm/flow_accumulation.hpp"
#include "lue/framework/algorithm/policy/all_values_within_domain.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"
#include "lue/framework/algorithm/policy/default_value_policies.hpp"
#include "lue/framework/partitioned_array.hpp"


namespace lue {
    namespace policy::upstream {

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

    }  // namespace upstream::policy


    template<
        typename Policies,
        typename FlowDirectionElement,
        typename MaterialElement,
        Rank rank>
    PartitionedArray<MaterialElement, rank> upstream(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
        PartitionedArray<MaterialElement, rank> const& material);


    template<
        typename FlowDirectionElement,
        typename MaterialElement,
        Rank rank>
    PartitionedArray<MaterialElement, rank> upstream(
        PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
        PartitionedArray<MaterialElement, rank> const& material)
    {
        using Policies = policy::upstream::DefaultPolicies<FlowDirectionElement, MaterialElement>;

        return upstream(Policies{}, flow_direction, material);
    }

}  // namespace lue
