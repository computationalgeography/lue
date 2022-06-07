#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array.hpp"


namespace lue {
    namespace policy::accu_threshold {

        template<
            typename FlowDirectionElement,
            typename MaterialElement>
        using DefaultPoliciesBase = Policies<
            AllValuesWithinDomain<FlowDirectionElement, MaterialElement, MaterialElement>,
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
                    SkipNoData<MaterialElement>,
                    FillHaloWithConstantValue<MaterialElement>>>>;


        template<
            typename FlowDirectionElement,
            typename MaterialElement>
        class DefaultPolicies:
            public DefaultPoliciesBase<FlowDirectionElement, MaterialElement>
        {

            public:

                using PoliciesBase = DefaultPoliciesBase<FlowDirectionElement, MaterialElement>;
                using FluxOutputPolicies = OutputPoliciesT<PoliciesBase, 0>;
                using StateOutputPolicies = OutputPoliciesT<PoliciesBase, 1>;
                using FlowDirectionInputPolicies = InputPoliciesT<PoliciesBase, 0>;
                using MaterialInputPolicies = InputPoliciesT<PoliciesBase, 1>;
                using ThresholdInputPolicies = InputPoliciesT<PoliciesBase, 2>;

                DefaultPolicies():

                    PoliciesBase{
                            DomainPolicyT<PoliciesBase>{},
                            FluxOutputPolicies{},
                            StateOutputPolicies{},
                            FlowDirectionInputPolicies{},
                            MaterialInputPolicies{FillHaloWithConstantValue<MaterialElement>{0}},
                            ThresholdInputPolicies{FillHaloWithConstantValue<MaterialElement>{0}}
                        }

                {
                }

        };


        template<
            typename FlowDirectionElement,
            typename MaterialElement>
        using DefaultValuePoliciesBase = Policies<
            // TODO Only accept non-negative material values!!!
            AllValuesWithinDomain<FlowDirectionElement, MaterialElement, MaterialElement>,
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
                    DetectNoDataByNaN<MaterialElement>,
                    FillHaloWithConstantValue<MaterialElement>>>>;


        template<
            typename FlowDirectionElement,
            typename MaterialElement>
        class DefaultValuePolicies:
            public DefaultValuePoliciesBase<FlowDirectionElement, MaterialElement>
        {

            public:

                using PoliciesBase = DefaultValuePoliciesBase<FlowDirectionElement, MaterialElement>;
                using FluxOutputPolicies = OutputPoliciesT<PoliciesBase, 0>;
                using StateOutputPolicies = OutputPoliciesT<PoliciesBase, 1>;
                using FlowDirectionInputPolicies = InputPoliciesT<PoliciesBase, 0>;
                using MaterialInputPolicies = InputPoliciesT<PoliciesBase, 1>;
                using ThresholdInputPolicies = InputPoliciesT<PoliciesBase, 2>;


                DefaultValuePolicies():

                    PoliciesBase{
                            DomainPolicyT<PoliciesBase>{},
                            FluxOutputPolicies{},
                            StateOutputPolicies{},
                            FlowDirectionInputPolicies{},
                            MaterialInputPolicies{FillHaloWithConstantValue<MaterialElement>{0}},
                            ThresholdInputPolicies{FillHaloWithConstantValue<MaterialElement>{0}}
                        }

                {
                }

        };

    }  // namespace policy::accu_threshold


    template<
        typename Policies,
        typename FlowDirectionElement,
        typename MaterialElement,
        Rank rank>
    std::tuple<
        PartitionedArray<MaterialElement, rank>,
        PartitionedArray<MaterialElement, rank>>
            accu_threshold(
                Policies const& policies,
                PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
                PartitionedArray<MaterialElement, rank> const& material,
                PartitionedArray<MaterialElement, rank> const& threshold);


    template<
        typename FlowDirectionElement,
        typename MaterialElement,
        Rank rank>
    std::tuple<
        PartitionedArray<MaterialElement, rank>,
        PartitionedArray<MaterialElement, rank>>
            accu_threshold(
                PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
                PartitionedArray<MaterialElement, rank> const& material,
                PartitionedArray<MaterialElement, rank> const& threshold)
    {
        using Policies = policy::accu_threshold::DefaultPolicies<FlowDirectionElement, MaterialElement>;

        return accu_threshold(Policies{}, flow_direction, material, threshold);
    }

}  // namespace lue
