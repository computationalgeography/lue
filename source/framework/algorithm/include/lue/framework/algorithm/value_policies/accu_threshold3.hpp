#pragma once
#include "lue/framework/algorithm/accu_threshold3.hpp"


namespace lue {
    namespace policy::accu_threshold3 {

        template<typename MaterialElement>
        class DomainPolicy
        {

            public:

                static constexpr bool within_domain(
                    MaterialElement const inflow, [[maybe_unused]] MaterialElement const threshold) noexcept
                {
                    return inflow >= 0;
                }
        };


        template<typename FlowDirectionElement, typename MaterialElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            DomainPolicy<MaterialElement>,
            OutputElements<MaterialElement, MaterialElement>,
            InputElements<FlowDirectionElement, MaterialElement, MaterialElement>>;

    }  // namespace policy::accu_threshold3


    namespace value_policies {

        template<typename FlowDirectionElement, typename MaterialElement, Rank rank>
        std::tuple<PartitionedArray<MaterialElement, rank>, PartitionedArray<MaterialElement, rank>>
        accu_threshold3(
            PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
            PartitionedArray<MaterialElement, rank> const& external_inflow,
            PartitionedArray<MaterialElement, rank> const& threshold)
        {
            using Policies =
                policy::accu_threshold3::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

            return accu_threshold3(Policies{}, flow_direction, external_inflow, threshold);
        }

    }  // namespace value_policies
}  // namespace lue
