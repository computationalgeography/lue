#pragma once
#include "lue/framework/algorithm/accu_threshold3.hpp"


namespace lue {
    namespace policy::accu_threshold3 {

        template<
            typename FlowDirectionElement,
            typename MaterialElement>
        using DefaultValuePoliciesBase = Policies<
            // TODO Only accept non-negative inflow values!!!
            AllValuesWithinDomain<FlowDirectionElement, MaterialElement, MaterialElement>,
            OutputsPolicies<
                OutputPolicies<MarkNoDataByNaN<MaterialElement>>,
                OutputPolicies<MarkNoDataByNaN<MaterialElement>>>,
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

    }  // namespace policy::accu_threshold3


    namespace value_policies {

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
                policy::accu_threshold3::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

            return accu_threshold3(Policies{}, flow_direction, external_inflow, threshold);
        }

    }  // namespace value_policies
}  // namespace lue
