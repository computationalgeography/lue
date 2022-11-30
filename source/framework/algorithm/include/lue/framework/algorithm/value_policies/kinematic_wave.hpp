#pragma once
#include "lue/framework/algorithm/kinematic_wave.hpp"


namespace lue {
    namespace policy::kinematic_wave {

        template<
            typename Element>
        class DomainPolicy
        {

            public:

                static constexpr bool within_domain(
                    Element const discharge,
                    Element const inflow,
                    Element const channel_length) noexcept
                {
                    return discharge >= 0 && inflow >= 0 && channel_length > 0;
                }

        };


        template<
            typename FlowDirectionElement,
            typename Element>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            DomainPolicy<Element>,
            OutputElements<Element>,
            InputElements<FlowDirectionElement, Element, Element, Element, Element, Element, Element>>;

    }  // namespace policy::kinematic_wave


    namespace value_policies {

        template<
            typename FlowDirectionElement,
            typename Element,
            Rank rank>
        PartitionedArray<Element, rank> kinematic_wave(
            PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
            PartitionedArray<Element, rank> const& discharge,
            PartitionedArray<Element, rank> const& inflow,
            Element const alpha,
            Element const beta,
            Element const time_step_duration,
            PartitionedArray<Element, rank> const& channel_length)
        {
            using Policies = policy::kinematic_wave::DefaultValuePolicies<FlowDirectionElement, Element>;

            return kinematic_wave(
                Policies{}, flow_direction, discharge, inflow, alpha, beta, time_step_duration, channel_length);
        }

    }  // namespace value_policies
}  // namespace lue
