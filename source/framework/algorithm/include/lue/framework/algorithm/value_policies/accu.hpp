#pragma once
#include "lue/framework/algorithm/accu.hpp"
#include <concepts>


namespace lue {
    namespace policy::accu {

        template<std::floating_point FloatingPointElement>
        class DomainPolicy
        {

            public:

                static constexpr auto within_domain(FloatingPointElement const inflow) noexcept -> bool
                {
                    return inflow >= 0;
                }
        };


        template<std::integral FlowDirectionElement, std::floating_point FloatingPointElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            DomainPolicy<FloatingPointElement>,
            OutputElements<FloatingPointElement>,
            InputElements<FlowDirectionElement, FloatingPointElement>>;

    }  // namespace policy::accu


    namespace value_policies {

        template<std::integral FlowDirectionElement, std::floating_point FloatingPointElement>
        auto accu(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            PartitionedArray<FloatingPointElement, 2> const& inflow)
            -> PartitionedArray<FloatingPointElement, 2>
        {
            using Policies =
                lue::policy::accu::DefaultValuePolicies<FlowDirectionElement, FloatingPointElement>;

            return accu(Policies{}, flow_direction, inflow);
        }


        template<std::integral FlowDirectionElement, std::floating_point FloatingPointElement>
        auto accu(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            Scalar<FloatingPointElement> const& inflow) -> PartitionedArray<FloatingPointElement, 2>
        {
            using Policies =
                lue::policy::accu::DefaultValuePolicies<FlowDirectionElement, FloatingPointElement>;

            return accu(Policies{}, flow_direction, inflow);
        }

    }  // namespace value_policies
}  // namespace lue
