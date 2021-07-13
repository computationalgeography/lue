#pragma once
#include "lue/framework/algorithm/accu3.hpp"


namespace lue {
    namespace policy::accu3 {

        template<
            typename FlowDirectionElement,
            typename MaterialElement>
        using DefaultValuePoliciesBase = Policies<
            // TODO Only accept non-negative inflow values!!!
            AllValuesWithinDomain<FlowDirectionElement, MaterialElement>,
            OutputsPolicies<
                OutputPolicies<MarkNoDataByNaN<MaterialElement>>>,
            InputsPolicies<
                SpatialOperationInputPolicies<
                    DetectNoDataByValue<FlowDirectionElement>,
                    FlowDirectionHalo<FlowDirectionElement>>,
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
                using FlowDirectionInputPolicies = InputPoliciesT<PoliciesBase, 0>;
                using MaterialInputPolicies = InputPoliciesT<PoliciesBase, 1>;


                DefaultValuePolicies():

                    PoliciesBase{
                            DomainPolicyT<PoliciesBase>{},
                            FluxOutputPolicies{},
                            FlowDirectionInputPolicies{},
                            MaterialInputPolicies{FillHaloWithConstantValue<MaterialElement>{0}}
                        }

                {
                }

        };

    }  // namespace policy::accu3


    namespace value_policies {

        template<
            typename FlowDirectionElement,
            typename MaterialElement,
            Rank rank>
        PartitionedArray<MaterialElement, rank> accu3(
            PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
            PartitionedArray<MaterialElement, rank> const& external_inflow)
        {
            using Policies = policy::accu3::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

            return accu3(Policies{}, flow_direction, external_inflow);
        }

    }  // namespace value_policies
}  // namespace lue
