#pragma once
#include "lue/framework/algorithm/kinematic_wave.hpp"
#include <concepts>


namespace lue {
    namespace policy::kinematic_wave {

        template<std::floating_point FloatingPointElement>
        class DomainPolicy
        {

            public:

                static constexpr auto within_domain(
                    FloatingPointElement const current_outflow,
                    FloatingPointElement const inflow,
                    FloatingPointElement const channel_length) noexcept -> bool
                {
                    return current_outflow >= 0 && inflow >= 0 && channel_length > 0;
                }
        };


        template<std::integral FlowDirectionElement, std::floating_point FloatingPointElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            DomainPolicy<FloatingPointElement>,
            OutputElements<FloatingPointElement>,
            InputElements<
                FlowDirectionElement,
                FloatingPointElement,
                FloatingPointElement,
                FloatingPointElement,
                FloatingPointElement,
                FloatingPointElement,
                FloatingPointElement>>;

    }  // namespace policy::kinematic_wave


    namespace value_policies {

        template<std::integral FlowDirectionElement, std::floating_point FloatingPointElement>
        auto kinematic_wave(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            PartitionedArray<FloatingPointElement, 2> const& current_outflow,
            PartitionedArray<FloatingPointElement, 2> const& inflow,
            PartitionedArray<FloatingPointElement, 2> const& alpha,
            PartitionedArray<FloatingPointElement, 2> const& beta,
            Scalar<FloatingPointElement> const& time_step_duration,
            PartitionedArray<FloatingPointElement, 2> const& channel_length)
            -> PartitionedArray<FloatingPointElement, 2>
        {
            using Policies =
                policy::kinematic_wave::DefaultValuePolicies<FlowDirectionElement, FloatingPointElement>;

            return kinematic_wave(
                Policies{},
                flow_direction,
                current_outflow,
                inflow,
                alpha,
                beta,
                time_step_duration,
                channel_length);
        }


        template<std::integral FlowDirectionElement, std::floating_point FloatingPointElement>
        auto kinematic_wave(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            PartitionedArray<FloatingPointElement, 2> const& current_outflow,
            PartitionedArray<FloatingPointElement, 2> const& inflow,
            Scalar<FloatingPointElement> const& alpha,
            Scalar<FloatingPointElement> const& beta,
            Scalar<FloatingPointElement> const& time_step_duration,
            Scalar<FloatingPointElement> const& channel_length) -> PartitionedArray<FloatingPointElement, 2>
        {
            using Policies =
                policy::kinematic_wave::DefaultValuePolicies<FlowDirectionElement, FloatingPointElement>;

            return kinematic_wave(
                Policies{},
                flow_direction,
                current_outflow,
                inflow,
                alpha,
                beta,
                time_step_duration,
                channel_length);
        }

    }  // namespace value_policies
}  // namespace lue
