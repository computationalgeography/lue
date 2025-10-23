#pragma once
#include "lue/framework/algorithm/partial_accu.hpp"
#include "lue/framework/algorithm/value_policies/accu.hpp"
#include <concepts>


namespace lue {

    namespace policy::partial_accu {

        using accu::DefaultValuePolicies;

    }  // namespace policy::partial_accu

    namespace value_policies {

        template<std::integral FlowDirectionElement, std::floating_point FloatingPointElement>
        auto partial_accu(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            Scalar<FloatingPointElement> const& inflow,
            Count const nr_steps) -> PartitionedArray<FloatingPointElement, 2>
        {
            using Policies =
                policy::partial_accu::DefaultValuePolicies<FlowDirectionElement, FloatingPointElement>;

            return partial_accu(Policies{}, flow_direction, inflow, nr_steps);
        }

    }  // namespace value_policies
}  // namespace lue
