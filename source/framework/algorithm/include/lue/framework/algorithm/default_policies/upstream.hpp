#pragma once
#include "lue/framework/algorithm/downstream.hpp"
#include "lue/framework/algorithm/flow_accumulation.hpp"
#include <concepts>


namespace lue {
    namespace policy::upstream {

        template<std::integral FlowDirectionElement, Arithmetic MaterialElement>
        using DefaultPolicies = flow_accumulation::DefaultPolicies<FlowDirectionElement, MaterialElement>;

    }  // namespace policy::upstream


    namespace default_policies {

        template<std::integral FlowDirectionElement, Arithmetic MaterialElement>
        auto upstream(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            PartitionedArray<MaterialElement, 2> const& material) -> PartitionedArray<MaterialElement, 2>
        {
            using Policies = policy::upstream::DefaultPolicies<FlowDirectionElement, MaterialElement>;

            return upstream(Policies{}, flow_direction, material);
        }

    }  // namespace default_policies
}  // namespace lue
