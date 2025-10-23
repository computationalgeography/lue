#pragma once
#include "lue/framework/algorithm/default_policies/accu.hpp"
#include "lue/framework/algorithm/partial_accu.hpp"
#include <concepts>


namespace lue {

    namespace policy::partial_accu {

        using accu::DefaultPolicies;

    }  // namespace policy::partial_accu

    namespace default_policies {

        template<std::integral FlowDirectionElement, std::floating_point FloatingPointElement>
        auto partial_accu(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            Scalar<FloatingPointElement> const& inflow,
            Count const nr_steps) -> PartitionedArray<FloatingPointElement, 2>
        {
            using Policies =
                policy::partial_accu::DefaultPolicies<FlowDirectionElement, FloatingPointElement>;

            return partial_accu(Policies{}, flow_direction, inflow, nr_steps);
        }

    }  // namespace default_policies
}  // namespace lue
