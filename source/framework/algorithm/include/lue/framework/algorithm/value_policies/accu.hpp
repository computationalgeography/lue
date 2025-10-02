#pragma once
#include "lue/framework/algorithm/accu.hpp"
#include <concepts>


namespace lue {
    namespace policy::accu {

        template<std::floating_point MaterialElement>
        class DomainPolicy
        {

            public:

                static constexpr auto within_domain(MaterialElement const inflow) noexcept -> bool
                {
                    return inflow >= 0;
                }
        };


        template<std::integral FlowDirectionElement, std::floating_point MaterialElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            DomainPolicy<MaterialElement>,
            OutputElements<MaterialElement>,
            InputElements<FlowDirectionElement, MaterialElement>>;

    }  // namespace policy::accu


    namespace value_policies {

        // raster
        template<std::integral FlowDirectionElement, std::floating_point MaterialElement>
        auto accu(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            PartitionedArray<MaterialElement, 2> const& inflow) -> PartitionedArray<MaterialElement, 2>
        {
            using Policies = lue::policy::accu::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

            return accu(Policies{}, flow_direction, inflow);
        }


        // scalar
        template<std::integral FlowDirectionElement, std::floating_point MaterialElement>
        auto accu(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            Scalar<MaterialElement> const& inflow) -> PartitionedArray<MaterialElement, 2>
        {
            using Policies = lue::policy::accu::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

            return accu(Policies{}, flow_direction, inflow);
        }


        // value
        template<std::integral FlowDirectionElement, std::floating_point MaterialElement>
        auto accu(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction, MaterialElement const& inflow)
            -> PartitionedArray<MaterialElement, 2>
        {
            using Policies = lue::policy::accu::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

            return accu(Policies{}, flow_direction, inflow);
        }

    }  // namespace value_policies
}  // namespace lue
