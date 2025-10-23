#pragma once
#include "lue/framework/algorithm/accu_threshold.hpp"
#include <concepts>


namespace lue {
    namespace policy::accu_threshold {

        template<std::integral FlowDirectionElement, std::floating_point FloatingPointElement>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<FloatingPointElement, FloatingPointElement>,
            OutputElements<FloatingPointElement, FloatingPointElement>,
            InputElements<FlowDirectionElement, FloatingPointElement, FloatingPointElement>>;

    }  // namespace policy::accu_threshold


    namespace default_policies {

        template<std::integral FlowDirectionElement, std::floating_point FloatingPointElement>
        auto accu_threshold(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            PartitionedArray<FloatingPointElement, 2> const& inflow,
            Scalar<FloatingPointElement> const& threshold) -> std::
            tuple<PartitionedArray<FloatingPointElement, 2>, PartitionedArray<FloatingPointElement, 2>>
        {
            using Policies =
                policy::accu_threshold::DefaultPolicies<FlowDirectionElement, FloatingPointElement>;

            return accu_threshold(Policies{}, flow_direction, inflow, threshold);
        }


        template<std::integral FlowDirectionElement, std::floating_point FloatingPointElement>
        auto accu_threshold(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            Scalar<FloatingPointElement> const& inflow,
            Scalar<FloatingPointElement> const& threshold) -> std::
            tuple<PartitionedArray<FloatingPointElement, 2>, PartitionedArray<FloatingPointElement, 2>>
        {
            using Policies =
                policy::accu_threshold::DefaultPolicies<FlowDirectionElement, FloatingPointElement>;

            return accu_threshold(Policies{}, flow_direction, inflow, threshold);
        }


        template<std::integral FlowDirectionElement, std::floating_point FloatingPointElement>
        auto accu_threshold(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            Scalar<FloatingPointElement> const& inflow,
            PartitionedArray<FloatingPointElement, 2> const& threshold) -> std::
            tuple<PartitionedArray<FloatingPointElement, 2>, PartitionedArray<FloatingPointElement, 2>>
        {
            using Policies =
                policy::accu_threshold::DefaultPolicies<FlowDirectionElement, FloatingPointElement>;

            return accu_threshold(Policies{}, flow_direction, inflow, threshold);
        }

    }  // namespace default_policies
}  // namespace lue
