#pragma once
#include "lue/framework/algorithm/kinematic_wave.hpp"
#include <concepts>


namespace lue {
    namespace policy::kinematic_wave {

        template<std::integral FlowDirectionElement, std::floating_point FloatingPointElement>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<FloatingPointElement, FloatingPointElement, FloatingPointElement>,
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


    namespace default_policies {

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
                policy::kinematic_wave::DefaultPolicies<FlowDirectionElement, FloatingPointElement>;

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
                policy::kinematic_wave::DefaultPolicies<FlowDirectionElement, FloatingPointElement>;

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

    }  // namespace default_policies
}  // namespace lue
