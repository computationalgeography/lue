#pragma once
#include "lue/framework/algorithm/accu.hpp"
#include <concepts>


namespace lue {
    namespace policy::accu {

        template<std::integral FlowDirectionElement, std::floating_point FloatingPointElement>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<FloatingPointElement>,
            OutputElements<FloatingPointElement>,
            InputElements<FlowDirectionElement, FloatingPointElement>>;

    }  // namespace policy::accu


    namespace default_policies {

        template<std::integral FlowDirectionElement, std::floating_point FloatingPointElement>
        auto accu(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            PartitionedArray<FloatingPointElement, 2> const& inflow)
            -> PartitionedArray<FloatingPointElement, 2>
        {
            using Policies = policy::accu::DefaultPolicies<FlowDirectionElement, FloatingPointElement>;

            return accu(Policies{}, flow_direction, inflow);
        }


        template<std::integral FlowDirectionElement, std::floating_point FloatingPointElement>
        auto accu(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            Scalar<FloatingPointElement> const& inflow) -> PartitionedArray<FloatingPointElement, 2>
        {
            using Policies = policy::accu::DefaultPolicies<FlowDirectionElement, FloatingPointElement>;

            return accu(Policies{}, flow_direction, inflow);
        }

    }  // namespace default_policies
}  // namespace lue
