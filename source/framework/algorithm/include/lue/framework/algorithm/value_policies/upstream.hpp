
#pragma once
#include "lue/framework/algorithm/flow_accumulation.hpp"
#include "lue/framework/algorithm/upstream.hpp"
#include <concepts>


namespace lue {
    namespace policy::upstream {

        template<std::integral FlowDirectionElement, Arithmetic MaterialElement>
        using DefaultValuePolicies =
            flow_accumulation::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

    }  // namespace policy::upstream


    namespace value_policies {

        template<std::integral FlowDirectionElement, Arithmetic MaterialElement>
        auto upstream(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            PartitionedArray<MaterialElement, 2> const& material) -> PartitionedArray<MaterialElement, 2>
        {
            using Policies = policy::upstream::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

            return upstream(Policies{}, flow_direction, material);
        }

    }  // namespace value_policies
}  // namespace lue
