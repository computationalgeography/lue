#pragma once
#include "lue/framework/algorithm/accu.hpp"
#include <concepts>


namespace lue {
    namespace policy::accu {

        template<std::integral FlowDirectionElement, std::floating_point MaterialElement>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<MaterialElement>,
            OutputElements<MaterialElement>,
            InputElements<FlowDirectionElement, MaterialElement>>;

    }  // namespace policy::accu


    namespace default_policies {

        // raster
        template<std::integral FlowDirectionElement, std::floating_point MaterialElement>
        auto accu(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            PartitionedArray<MaterialElement, 2> const& inflow) -> PartitionedArray<MaterialElement, 2>
        {
            using Policies = lue::policy::accu::DefaultPolicies<FlowDirectionElement, MaterialElement>;

            return accu(Policies{}, flow_direction, inflow);
        }


        // scalar
        template<std::integral FlowDirectionElement, std::floating_point MaterialElement>
        auto accu(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            Scalar<MaterialElement> const& inflow) -> PartitionedArray<MaterialElement, 2>
        {
            using Policies = lue::policy::accu::DefaultPolicies<FlowDirectionElement, MaterialElement>;

            return accu(Policies{}, flow_direction, inflow);
        }


        // value
        template<std::integral FlowDirectionElement, std::floating_point MaterialElement>
        auto accu(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction, MaterialElement const& inflow)
            -> PartitionedArray<MaterialElement, 2>
        {
            using Policies = lue::policy::accu::DefaultPolicies<FlowDirectionElement, MaterialElement>;

            return accu(Policies{}, flow_direction, inflow);
        }

    }  // namespace default_policies
}  // namespace lue
