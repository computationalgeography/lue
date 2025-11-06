#pragma once
#include "lue/framework/algorithm/d8_flow_direction.hpp"
#include "lue/framework/configure.hpp"
#include <concepts>


namespace lue {
    namespace policy::d8_flow_direction {

        template<std::integral FlowDirectionElement, Arithmetic ElevationElement>
        using DefaultValuePoliciesBase = Policies<
            AllValuesWithinDomain<ElevationElement>,
            OutputsPolicies<OutputPolicies<
                DefaultOutputNoDataPolicy<FlowDirectionElement>,
                AllValuesWithinRange<FlowDirectionElement>>>,
            InputsPolicies<SpatialOperationInputPolicies<
                DefaultInputNoDataPolicy<ElevationElement>,
                FillHaloWithConstantValue<ElevationElement>>>>;


        template<std::integral FlowDirectionElement, Arithmetic ElevationElement>
        class DefaultValuePolicies: public DefaultValuePoliciesBase<FlowDirectionElement, ElevationElement>
        {

            public:

                using PoliciesBase = DefaultValuePoliciesBase<FlowDirectionElement, ElevationElement>;
                using FlowDirectionOutputPolicies = OutputPoliciesT<PoliciesBase, 0>;
                using ElevationInputPolicies = InputPoliciesT<PoliciesBase, 0>;


                DefaultValuePolicies():

                    PoliciesBase{
                        DomainPolicyT<PoliciesBase>{},
                        FlowDirectionOutputPolicies{},
                        ElevationInputPolicies{FillHaloWithConstantValue<ElevationElement>{
                            std::numeric_limits<ElevationElement>::min()}}}

                {
                }
        };

    }  // namespace policy::d8_flow_direction


    namespace value_policies {

        template<Arithmetic ElevationElement>
        auto d8_flow_direction(PartitionedArray<ElevationElement, 2> const& elevation)
            -> PartitionedArray<FlowDirectionElement, 2>
        {
            using Policies =
                policy::d8_flow_direction::DefaultValuePolicies<FlowDirectionElement, ElevationElement>;

            return d8_flow_direction(Policies{}, elevation);
        }

    }  // namespace value_policies
}  // namespace lue
