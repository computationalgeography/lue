#pragma once
#include "lue/framework/algorithm/accu3.hpp"


namespace lue {
    namespace policy::accu3 {

        template<
            typename MaterialElement>
        class DomainPolicy
        {

            public:

                static constexpr bool within_domain(
                    MaterialElement const inflow) noexcept
                {
                    return inflow >= 0;
                }

        };


        template<
            typename FlowDirectionElement,
            typename MaterialElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            DomainPolicy<MaterialElement>,
            OutputElements<MaterialElement>,
            InputElements<FlowDirectionElement, MaterialElement>>;

    }  // namespace policy::accu3


    namespace value_policies {

        template<
            typename FlowDirectionElement,
            typename MaterialElement,
            Rank rank>
        PartitionedArray<MaterialElement, rank> accu3(
            PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
            PartitionedArray<MaterialElement, rank> const& external_inflow)
        {
            using Policies = policy::accu3::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

            return accu3(Policies{}, flow_direction, external_inflow);
        }

    }  // namespace value_policies
}  // namespace lue
