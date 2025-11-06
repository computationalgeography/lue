#pragma once
#include "lue/framework/algorithm/d8_flow_direction.hpp"
#include "lue/framework/configure.hpp"
#include <concepts>


namespace lue {
    namespace policy::d8_flow_direction {

        template<std::integral FlowDirectionElement, Arithmetic ElevationElement>
        using DefaultPoliciesBase = Policies<
            AllValuesWithinDomain<ElevationElement>,
            OutputsPolicies<OutputPolicies<
                DontMarkNoData<FlowDirectionElement>,
                AllValuesWithinRange<FlowDirectionElement>>>,
            InputsPolicies<SpatialOperationInputPolicies<
                SkipNoData<ElevationElement>,
                FillHaloWithConstantValue<ElevationElement>>>>;


        template<std::integral FlowDirectionElement, Arithmetic ElevationElement>
        class DefaultPolicies: public DefaultPoliciesBase<FlowDirectionElement, ElevationElement>
        {

            public:

                using PoliciesBase = DefaultPoliciesBase<FlowDirectionElement, ElevationElement>;
                using FlowDirectionOutputPolicies = OutputPoliciesT<PoliciesBase, 0>;
                using ElevationInputPolicies = InputPoliciesT<PoliciesBase, 0>;


                DefaultPolicies():

                    PoliciesBase{
                        DomainPolicyT<PoliciesBase>{},
                        FlowDirectionOutputPolicies{},
                        ElevationInputPolicies{FillHaloWithConstantValue<ElevationElement>{
                            std::numeric_limits<ElevationElement>::min()}}}

                {
                }
        };

    }  // namespace policy::d8_flow_direction


    namespace default_policies {

        template<Arithmetic ElevationElement>
        auto d8_flow_direction(PartitionedArray<ElevationElement, 2> const& elevation)
            -> PartitionedArray<FlowDirectionElement, 2>
        {
            using Policies =
                policy::d8_flow_direction::DefaultPolicies<FlowDirectionElement, ElevationElement>;

            return d8_flow_direction(Policies{}, elevation);
        }

    }  // namespace default_policies
}  // namespace lue
