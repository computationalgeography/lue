#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array.hpp"
#include <type_traits>


namespace lue {
    namespace policy::d8_flow_direction {

        template<
            typename FlowDirectionElement,
            typename ElevationElement>
        using DefaultPoliciesBase = Policies<
            AllValuesWithinDomain<ElevationElement>,
            OutputsPolicies<
                OutputPolicies<DontMarkNoData<FlowDirectionElement>>>,
            InputsPolicies<
                SpatialOperationInputPolicies<
                    SkipNoData<ElevationElement>,
                    FillHaloWithConstantValue<ElevationElement>>>>;


        template<
            typename FlowDirectionElement,
            typename ElevationElement>
        class DefaultPolicies:
            public DefaultPoliciesBase<FlowDirectionElement, ElevationElement>
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
                                std::numeric_limits<ElevationElement>::min()}}
                        }

                {
                }

        };


        template<
            typename FlowDirectionElement,
            typename ElevationElement>
        using DefaultValuePoliciesBase = Policies<
            AllValuesWithinDomain<ElevationElement>,
            OutputsPolicies<
                OutputPolicies<DefaultOutputNoDataPolicy<FlowDirectionElement>>>,
            InputsPolicies<
                SpatialOperationInputPolicies<
                    DefaultInputNoDataPolicy<ElevationElement>,
                    FillHaloWithConstantValue<ElevationElement>>>>;


        template<
            typename FlowDirectionElement,
            typename ElevationElement>
        class DefaultValuePolicies:
            public DefaultValuePoliciesBase<FlowDirectionElement, ElevationElement>
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
                                std::numeric_limits<ElevationElement>::min()}}
                        }

                {
                }

        };

    }  // namespace policy::d8_flow_direction


    template<
        typename FlowDirectionElement,
        typename Policies,
        typename ElevationElement,
        Rank rank>
    PartitionedArray<FlowDirectionElement, rank> d8_flow_direction(
        Policies const& policies,
        PartitionedArray<ElevationElement, rank> const& elevation);


    template<
        typename FlowDirectionElement,
        typename ElevationElement,
        Rank rank>
    PartitionedArray<FlowDirectionElement, rank> d8_flow_direction(
        PartitionedArray<ElevationElement, rank> const& elevation)
    {
        using Policies = policy::d8_flow_direction::DefaultPolicies<FlowDirectionElement, ElevationElement>;

        return d8_flow_direction<FlowDirectionElement>(Policies{}, elevation);
    }

}  // namespace lue
