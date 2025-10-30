#pragma once
#include "lue/framework/algorithm/accu_threshold.hpp"
#include "lue/framework.hpp"
#include <concepts>


namespace lue {
    namespace policy::accu_threshold {

        template<std::floating_point FloatingPointElement>
        class DomainPolicy
        {

            public:

                static constexpr auto within_domain(
                    FloatingPointElement const inflow, FloatingPointElement const threshold) noexcept -> bool
                {
                    return inflow >= 0 && threshold >= 0;
                }
        };


        template<std::integral FlowDirectionElement, std::floating_point FloatingPointElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            DomainPolicy<FloatingPointElement>,
            OutputElements<FloatingPointElement, FloatingPointElement>,
            InputElements<FlowDirectionElement, FloatingPointElement, FloatingPointElement>>;

    }  // namespace policy::accu_threshold


    namespace value_policies {

        template<std::floating_point FloatingPointElement>
        auto accu_threshold(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            PartitionedArray<FloatingPointElement, 2> const& inflow,
            PartitionedArray<FloatingPointElement, 2> const& threshold) -> std::
            tuple<PartitionedArray<FloatingPointElement, 2>, PartitionedArray<FloatingPointElement, 2>>
        {
            using Policies =
                policy::accu_threshold::DefaultValuePolicies<FlowDirectionElement, FloatingPointElement>;

            return accu_threshold(Policies{}, flow_direction, inflow, threshold);
        }


        template<std::floating_point FloatingPointElement>
        auto accu_threshold(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            PartitionedArray<FloatingPointElement, 2> const& inflow,
            Scalar<FloatingPointElement> const& threshold) -> std::
            tuple<PartitionedArray<FloatingPointElement, 2>, PartitionedArray<FloatingPointElement, 2>>
        {
            using Policies =
                policy::accu_threshold::DefaultValuePolicies<FlowDirectionElement, FloatingPointElement>;

            return accu_threshold(Policies{}, flow_direction, inflow, threshold);
        }


        template<std::floating_point FloatingPointElement>
        auto accu_threshold(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            Scalar<FloatingPointElement> const& inflow,
            Scalar<FloatingPointElement> const& threshold) -> std::
            tuple<PartitionedArray<FloatingPointElement, 2>, PartitionedArray<FloatingPointElement, 2>>
        {
            using Policies =
                policy::accu_threshold::DefaultValuePolicies<FlowDirectionElement, FloatingPointElement>;

            return accu_threshold(Policies{}, flow_direction, inflow, threshold);
        }


        template<std::floating_point FloatingPointElement>
        auto accu_threshold(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            Scalar<FloatingPointElement> const& inflow,
            PartitionedArray<FloatingPointElement, 2> const& threshold) -> std::
            tuple<PartitionedArray<FloatingPointElement, 2>, PartitionedArray<FloatingPointElement, 2>>
        {
            using Policies =
                policy::accu_threshold::DefaultValuePolicies<FlowDirectionElement, FloatingPointElement>;

            return accu_threshold(Policies{}, flow_direction, inflow, threshold);
        }

    }  // namespace value_policies
}  // namespace lue
