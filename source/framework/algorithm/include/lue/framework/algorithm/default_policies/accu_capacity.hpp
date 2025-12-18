#pragma once
#include "lue/framework/algorithm/accu_capacity.hpp"
#include <concepts>


namespace lue {
    namespace policy::accu_capacity {

        template<std::integral FlowDirectionElement, std::floating_point FloatingPointElement>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<FloatingPointElement, FloatingPointElement>,
            OutputElements<FloatingPointElement, FloatingPointElement>,
            InputElements<FlowDirectionElement, FloatingPointElement, FloatingPointElement>>;

    }  // namespace policy::accu_capacity


    namespace default_policies {

        template<std::integral FlowDirectionElement, std::floating_point FloatingPointElement>
        auto accu_capacity(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            PartitionedArray<FloatingPointElement, 2> const& inflow,
            Scalar<FloatingPointElement> const& capacity) -> std::
            tuple<PartitionedArray<FloatingPointElement, 2>, PartitionedArray<FloatingPointElement, 2>>
        {
            using Policies =
                policy::accu_capacity::DefaultPolicies<FlowDirectionElement, FloatingPointElement>;

            return accu_capacity(Policies{}, flow_direction, inflow, capacity);
        }


        template<std::integral FlowDirectionElement, std::floating_point FloatingPointElement>
        auto accu_capacity(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            Scalar<FloatingPointElement> const& inflow,
            Scalar<FloatingPointElement> const& capacity) -> std::
            tuple<PartitionedArray<FloatingPointElement, 2>, PartitionedArray<FloatingPointElement, 2>>
        {
            using Policies =
                policy::accu_capacity::DefaultPolicies<FlowDirectionElement, FloatingPointElement>;

            return accu_capacity(Policies{}, flow_direction, inflow, capacity);
        }


        template<std::integral FlowDirectionElement, std::floating_point FloatingPointElement>
        auto accu_capacity(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            Scalar<FloatingPointElement> const& inflow,
            PartitionedArray<FloatingPointElement, 2> const& capacity) -> std::
            tuple<PartitionedArray<FloatingPointElement, 2>, PartitionedArray<FloatingPointElement, 2>>
        {
            using Policies =
                policy::accu_capacity::DefaultPolicies<FlowDirectionElement, FloatingPointElement>;

            return accu_capacity(Policies{}, flow_direction, inflow, capacity);
        }

    }  // namespace default_policies
}  // namespace lue
