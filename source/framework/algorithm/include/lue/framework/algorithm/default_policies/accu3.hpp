#pragma once
#include "lue/framework/algorithm/accu3.hpp"


namespace lue {
    namespace policy::accu3 {

        template<typename FlowDirectionElement, typename MaterialElement>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<MaterialElement>,
            OutputElements<MaterialElement>,
            InputElements<FlowDirectionElement, MaterialElement>>;

    }  // namespace policy::accu3


    namespace default_policies {

        template<typename FlowDirectionElement, typename MaterialElement, Rank rank>
        PartitionedArray<MaterialElement, rank> accu3(
            PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
            PartitionedArray<MaterialElement, rank> const& external_inflow)
        {
            using Policies = policy::accu3::DefaultPolicies<FlowDirectionElement, MaterialElement>;

            return accu3(Policies{}, flow_direction, external_inflow);
        }

    }  // namespace default_policies
}  // namespace lue
