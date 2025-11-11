#pragma once
#include "lue/framework/algorithm/accu_fraction.hpp"
#include <concepts>


namespace lue {
    namespace policy::accu_fraction {

        template<std::floating_point FloatingPointElement>
        class DomainPolicy
        {

            public:

                static constexpr auto within_domain(
                    FloatingPointElement const inflow, FloatingPointElement const fraction) noexcept -> bool
                {
                    return inflow >= 0 && fraction >= 0 && fraction <= 1;
                }
        };


        template<std::integral FlowDirectionElement, std::floating_point FloatingPointElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            DomainPolicy<FloatingPointElement>,
            OutputElements<FloatingPointElement, FloatingPointElement>,
            InputElements<FlowDirectionElement, FloatingPointElement, FloatingPointElement>>;

    }  // namespace policy::accu_fraction


    namespace value_policies {

        template<std::integral FlowDirectionElement, std::floating_point FloatingPointElement>
        auto accu_fraction(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            PartitionedArray<FloatingPointElement, 2> const& inflow,
            PartitionedArray<FloatingPointElement, 2> const& fraction) -> std::
            tuple<PartitionedArray<FloatingPointElement, 2>, PartitionedArray<FloatingPointElement, 2>>
        {
            using Policies =
                policy::accu_fraction::DefaultValuePolicies<FlowDirectionElement, FloatingPointElement>;

            return accu_fraction(Policies{}, flow_direction, inflow, fraction);
        }


        template<std::integral FlowDirectionElement, std::floating_point FloatingPointElement>
        auto accu_fraction(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            PartitionedArray<FloatingPointElement, 2> const& inflow,
            Scalar<FloatingPointElement> const& fraction) -> std::
            tuple<PartitionedArray<FloatingPointElement, 2>, PartitionedArray<FloatingPointElement, 2>>
        {
            using Policies =
                policy::accu_fraction::DefaultValuePolicies<FlowDirectionElement, FloatingPointElement>;

            return accu_fraction(Policies{}, flow_direction, inflow, fraction);
        }


        template<std::integral FlowDirectionElement, std::floating_point FloatingPointElement>
        auto accu_fraction(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            Scalar<FloatingPointElement> const& inflow,
            Scalar<FloatingPointElement> const& fraction) -> std::
            tuple<PartitionedArray<FloatingPointElement, 2>, PartitionedArray<FloatingPointElement, 2>>
        {
            using Policies =
                policy::accu_fraction::DefaultValuePolicies<FlowDirectionElement, FloatingPointElement>;

            return accu_fraction(Policies{}, flow_direction, inflow, fraction);
        }


        template<std::integral FlowDirectionElement, std::floating_point FloatingPointElement>
        auto accu_fraction(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            Scalar<FloatingPointElement> const& inflow,
            PartitionedArray<FloatingPointElement, 2> const& fraction) -> std::
            tuple<PartitionedArray<FloatingPointElement, 2>, PartitionedArray<FloatingPointElement, 2>>
        {
            using Policies =
                policy::accu_fraction::DefaultValuePolicies<FlowDirectionElement, FloatingPointElement>;

            return accu_fraction(Policies{}, flow_direction, inflow, fraction);
        }

    }  // namespace value_policies
}  // namespace lue
