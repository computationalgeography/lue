#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array.hpp"


namespace lue {
    namespace policy::accu_fraction {

        template<
            typename FlowDirectionElement,
            typename MaterialElement,
            typename FractionElement>
        using DefaultPoliciesBase = Policies<
            AllValuesWithinDomain<FlowDirectionElement, MaterialElement, FractionElement>,
            OutputsPolicies<
                    OutputPolicies<
                            DontMarkNoData<MaterialElement>,
                            AllValuesWithinRange<MaterialElement>
                        >,
                    OutputPolicies<
                            DontMarkNoData<MaterialElement>,
                            AllValuesWithinRange<MaterialElement>
                        >
                >,
            InputsPolicies<
                SpatialOperationInputPolicies<
                    SkipNoData<FlowDirectionElement>,
                    FlowDirectionHalo<FlowDirectionElement>>,
                SpatialOperationInputPolicies<
                    SkipNoData<MaterialElement>,
                    FillHaloWithConstantValue<MaterialElement>>,
                SpatialOperationInputPolicies<
                    SkipNoData<FractionElement>,
                    FillHaloWithConstantValue<FractionElement>>>>;


        template<
            typename FlowDirectionElement,
            typename MaterialElement,
            typename FractionElement>
        class DefaultPolicies:
            public DefaultPoliciesBase<FlowDirectionElement, MaterialElement, FractionElement>
        {

            public:

                using PoliciesBase =
                    DefaultPoliciesBase<FlowDirectionElement, MaterialElement, FractionElement>;
                using FluxOutputPolicies = OutputPoliciesT<PoliciesBase, 0>;
                using StateOutputPolicies = OutputPoliciesT<PoliciesBase, 1>;
                using FlowDirectionInputPolicies = InputPoliciesT<PoliciesBase, 0>;
                using MaterialInputPolicies = InputPoliciesT<PoliciesBase, 1>;
                using FractionInputPolicies = InputPoliciesT<PoliciesBase, 2>;


                DefaultPolicies():

                    PoliciesBase{
                            DomainPolicyT<PoliciesBase>{},
                            FluxOutputPolicies{},
                            StateOutputPolicies{},
                            FlowDirectionInputPolicies{},
                            MaterialInputPolicies{FillHaloWithConstantValue<MaterialElement>{0}},
                            FractionInputPolicies{FillHaloWithConstantValue<FractionElement>{0}}
                        }

                {
                }

        };


        template<
            typename FlowDirectionElement,
            typename MaterialElement,
            typename FractionElement>
        using DefaultValuePoliciesBase = Policies<
            // TODO Only accept non-negative material values!!!
            AllValuesWithinDomain<FlowDirectionElement, MaterialElement, FractionElement>,
            OutputsPolicies<
                    OutputPolicies<
                            MarkNoDataByNaN<MaterialElement>,
                            AllValuesWithinRange<MaterialElement>
                        >,
                    OutputPolicies<
                            MarkNoDataByNaN<MaterialElement>,
                            AllValuesWithinRange<MaterialElement>
                        >
                >,
            InputsPolicies<
                SpatialOperationInputPolicies<
                    DetectNoDataByValue<FlowDirectionElement>,
                    FlowDirectionHalo<FlowDirectionElement>>,
                SpatialOperationInputPolicies<
                    DetectNoDataByNaN<MaterialElement>,
                    FillHaloWithConstantValue<MaterialElement>>,
                SpatialOperationInputPolicies<
                    DetectNoDataByNaN<FractionElement>,
                    FillHaloWithConstantValue<FractionElement>>>>;


        template<
            typename FlowDirectionElement,
            typename MaterialElement,
            typename FractionElement>
        class DefaultValuePolicies:
            public DefaultValuePoliciesBase<FlowDirectionElement, MaterialElement, FractionElement>
        {

            public:

                using PoliciesBase =
                    DefaultValuePoliciesBase<FlowDirectionElement, MaterialElement, FractionElement>;
                using FluxOutputPolicies = OutputPoliciesT<PoliciesBase, 0>;
                using StateOutputPolicies = OutputPoliciesT<PoliciesBase, 1>;
                using FlowDirectionInputPolicies = InputPoliciesT<PoliciesBase, 0>;
                using MaterialInputPolicies = InputPoliciesT<PoliciesBase, 1>;
                using FractionInputPolicies = InputPoliciesT<PoliciesBase, 2>;


                DefaultValuePolicies():

                    PoliciesBase{
                            DomainPolicyT<PoliciesBase>{},
                            FluxOutputPolicies{},
                            StateOutputPolicies{},
                            FlowDirectionInputPolicies{},
                            MaterialInputPolicies{FillHaloWithConstantValue<MaterialElement>{0}},
                            FractionInputPolicies{FillHaloWithConstantValue<FractionElement>{0}}
                        }

                {
                }

        };

    }  // namespace policy::accu_fraction


    template<
        typename Policies,
        typename FlowDirectionElement,
        typename MaterialElement,
        typename FractionElement,
        Rank rank>
    std::tuple<
        PartitionedArray<MaterialElement, rank>,
        PartitionedArray<MaterialElement, rank>>
            accu_fraction(
                Policies const& policies,
                PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
                PartitionedArray<MaterialElement, rank> const& material,
                PartitionedArray<FractionElement, rank> const& fraction);


    template<
        typename FlowDirectionElement,
        typename MaterialElement,
        typename FractionElement,
        Rank rank>
    std::tuple<
        PartitionedArray<MaterialElement, rank>,
        PartitionedArray<MaterialElement, rank>>
            accu_fraction(
                PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
                PartitionedArray<MaterialElement, rank> const& material,
                PartitionedArray<FractionElement, rank> const& fraction)
    {
        using Policies =
            policy::accu_fraction::DefaultPolicies<FlowDirectionElement, MaterialElement, FractionElement>;

        return accu_fraction(Policies{}, flow_direction, material, fraction);
    }

}  // namespace lue
