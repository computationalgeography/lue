
#pragma once
#include "lue/framework/algorithm/downstream.hpp"
#include "lue/framework/algorithm/flow_accumulation.hpp"
#include <concepts>


namespace lue {
    namespace policy::downstream {

        template<std::integral FlowDirectionElement, Arithmetic MaterialElement>
        using DefaultValuePolicies =
            flow_accumulation::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

    }  // namespace policy::downstream


    namespace value_policies {

        template<std::integral FlowDirectionElement, Arithmetic MaterialElement>
        auto downstream(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            PartitionedArray<MaterialElement, 2> const& material) -> PartitionedArray<MaterialElement, 2>
        {
            using Policies = policy::downstream::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

            return downstream(Policies{}, flow_direction, material);
        }

    }  // namespace value_policies
}  // namespace lue
