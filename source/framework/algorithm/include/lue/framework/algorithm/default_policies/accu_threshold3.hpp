#pragma once
#include "lue/framework/algorithm/accu_threshold3.hpp"


namespace lue {
    namespace policy::accu_threshold3 {

        template<
            typename FlowDirectionElement,
            typename MaterialElement>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<MaterialElement, MaterialElement>,
            OutputElements<MaterialElement, MaterialElement>,
            InputElements<FlowDirectionElement, MaterialElement, MaterialElement>>;

    }  // namespace policy::accu_threshold3


    namespace default_policies {

        template<
            typename FlowDirectionElement,
            typename MaterialElement,
            Rank rank>
        std::tuple<
            PartitionedArray<MaterialElement, rank>,
            PartitionedArray<MaterialElement, rank>>
                accu_threshold3(
                    PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
                    PartitionedArray<MaterialElement, rank> const& external_inflow,
                    PartitionedArray<MaterialElement, rank> const& threshold)
        {
            using Policies =
                policy::accu_threshold3::DefaultPolicies<FlowDirectionElement, MaterialElement>;

            return accu_threshold3(Policies{}, flow_direction, external_inflow, threshold);
        }

    }  // namespace default_policies
}  // namespace lue
