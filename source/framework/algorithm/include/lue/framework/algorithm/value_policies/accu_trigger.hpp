#pragma once
#include "lue/framework/algorithm/accu_trigger.hpp"
#include <concepts>


namespace lue {
    namespace policy::accu_trigger {

        template<std::floating_point FloatingPointElement>
        class DomainPolicy
        {

            public:

                static constexpr auto within_domain(
                    FloatingPointElement const inflow, FloatingPointElement const trigger) noexcept -> bool
                {
                    return inflow >= 0 && trigger >= 0;
                }
        };


        template<std::integral FlowDirectionElement, std::floating_point FloatingPointElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            DomainPolicy<FloatingPointElement>,
            OutputElements<FloatingPointElement, FloatingPointElement>,
            InputElements<FlowDirectionElement, FloatingPointElement, FloatingPointElement>>;

    }  // namespace policy::accu_trigger


    namespace value_policies {

        template<std::integral FlowDirectionElement, std::floating_point FloatingPointElement>
        auto accu_trigger(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            PartitionedArray<FloatingPointElement, 2> const& inflow,
            PartitionedArray<FloatingPointElement, 2> const& trigger) -> std::
            tuple<PartitionedArray<FloatingPointElement, 2>, PartitionedArray<FloatingPointElement, 2>>
        {
            using Policies =
                policy::accu_trigger::DefaultValuePolicies<FlowDirectionElement, FloatingPointElement>;

            return accu_trigger(Policies{}, flow_direction, inflow, trigger);
        }


        template<std::integral FlowDirectionElement, std::floating_point FloatingPointElement>
        auto accu_trigger(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            PartitionedArray<FloatingPointElement, 2> const& inflow,
            Scalar<FloatingPointElement> const& trigger) -> std::
            tuple<PartitionedArray<FloatingPointElement, 2>, PartitionedArray<FloatingPointElement, 2>>
        {
            using Policies =
                policy::accu_trigger::DefaultValuePolicies<FlowDirectionElement, FloatingPointElement>;

            return accu_trigger(Policies{}, flow_direction, inflow, trigger);
        }


        template<std::integral FlowDirectionElement, std::floating_point FloatingPointElement>
        auto accu_trigger(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            Scalar<FloatingPointElement> const& inflow,
            Scalar<FloatingPointElement> const& trigger) -> std::
            tuple<PartitionedArray<FloatingPointElement, 2>, PartitionedArray<FloatingPointElement, 2>>
        {
            using Policies =
                policy::accu_trigger::DefaultValuePolicies<FlowDirectionElement, FloatingPointElement>;

            return accu_trigger(Policies{}, flow_direction, inflow, trigger);
        }


        template<std::integral FlowDirectionElement, std::floating_point FloatingPointElement>
        auto accu_trigger(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            Scalar<FloatingPointElement> const& inflow,
            PartitionedArray<FloatingPointElement, 2> const& trigger) -> std::
            tuple<PartitionedArray<FloatingPointElement, 2>, PartitionedArray<FloatingPointElement, 2>>
        {
            using Policies =
                policy::accu_trigger::DefaultValuePolicies<FlowDirectionElement, FloatingPointElement>;

            return accu_trigger(Policies{}, flow_direction, inflow, trigger);
        }

    }  // namespace value_policies
}  // namespace lue
