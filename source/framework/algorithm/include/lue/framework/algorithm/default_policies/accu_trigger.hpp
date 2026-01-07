#pragma once
#include "lue/framework/algorithm/accu_trigger.hpp"
#include <concepts>


namespace lue {
    namespace policy::accu_trigger {

        template<std::integral FlowDirectionElement, std::floating_point FloatingPointElement>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<FloatingPointElement, FloatingPointElement>,
            OutputElements<FloatingPointElement, FloatingPointElement>,
            InputElements<FlowDirectionElement, FloatingPointElement, FloatingPointElement>>;

    }  // namespace policy::accu_trigger


    namespace default_policies {

        template<std::integral FlowDirectionElement, std::floating_point FloatingPointElement>
        auto accu_trigger(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            PartitionedArray<FloatingPointElement, 2> const& inflow,
            Scalar<FloatingPointElement> const& trigger) -> std::
            tuple<PartitionedArray<FloatingPointElement, 2>, PartitionedArray<FloatingPointElement, 2>>
        {
            using Policies =
                policy::accu_trigger::DefaultPolicies<FlowDirectionElement, FloatingPointElement>;

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
                policy::accu_trigger::DefaultPolicies<FlowDirectionElement, FloatingPointElement>;

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
                policy::accu_trigger::DefaultPolicies<FlowDirectionElement, FloatingPointElement>;

            return accu_trigger(Policies{}, flow_direction, inflow, trigger);
        }

    }  // namespace default_policies
}  // namespace lue
