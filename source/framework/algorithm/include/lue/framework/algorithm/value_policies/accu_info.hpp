#pragma once
#include "lue/framework/algorithm/accu_info.hpp"
#include <concepts>


namespace lue {
    namespace policy::accu_info {

        template<std::integral FlowDirectionElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<>,
            OutputElements<CellClass>,
            InputElements<FlowDirectionElement>>;

    }  // namespace policy::accu_info


    namespace value_policies {

        template<std::integral FlowDirectionElement>
        auto accu_info(PartitionedArray<FlowDirectionElement, 2> const& flow_direction)
            -> PartitionedArray<CellClass, 2>
        {
            using Policies = policy::accu_info::DefaultValuePolicies<FlowDirectionElement>;

            return accu_info(Policies{}, flow_direction);
        }

    }  // namespace value_policies
}  // namespace lue
