#pragma once
#include "lue/framework/algorithm/policy.hpp"


namespace lue {
    namespace policy::flow_accumulation {

        template<
            typename FlowDirectionElement,
            typename MaterialElement>
        using DefaultPoliciesBase = Policies<
            AllValuesWithinDomain<FlowDirectionElement, MaterialElement>,
            OutputsPolicies<
                    OutputPolicies<
                            DontMarkNoData<MaterialElement>,
                            AllValuesWithinRange<MaterialElement, FlowDirectionElement>
                        >
                >,
            InputsPolicies<
                SpatialOperationInputPolicies<
                    SkipNoData<FlowDirectionElement>,
                    FlowDirectionHalo<FlowDirectionElement>>,
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
                using MaterialOutputPolicies = OutputPoliciesT<PoliciesBase, 0>;
                using FlowDirectionInputPolicies = InputPoliciesT<PoliciesBase, 0>;
                using MaterialInputPolicies = InputPoliciesT<PoliciesBase, 1>;

                DefaultPolicies():

                    PoliciesBase{
                            DomainPolicyT<PoliciesBase>{},
                            MaterialOutputPolicies{},
                            FlowDirectionInputPolicies{},
                            MaterialInputPolicies{FillHaloWithConstantValue<MaterialElement>{0}}
                        }

                {
                }

        };


        template<
            typename FlowDirectionElement,
            typename MaterialElement>
        using DefaultValuePoliciesBase = Policies<
            // TODO Only accept non-negative material values!!!
            AllValuesWithinDomain<FlowDirectionElement, MaterialElement>,
            OutputsPolicies<
                    OutputPolicies<
                            DefaultOutputNoDataPolicy<MaterialElement>,
                            AllValuesWithinRange<MaterialElement, FlowDirectionElement>
                        >
                >,
            InputsPolicies<
                SpatialOperationInputPolicies<
                    DetectNoDataByValue<FlowDirectionElement>,
                    FlowDirectionHalo<FlowDirectionElement>>,
                SpatialOperationInputPolicies<
                    DefaultInputNoDataPolicy<MaterialElement>,
                    FillHaloWithConstantValue<MaterialElement>>>>;


        template<
            typename FlowDirectionElement,
            typename MaterialElement>
        class DefaultValuePolicies:
            public DefaultValuePoliciesBase<FlowDirectionElement, MaterialElement>
        {

            public:

                using PoliciesBase = DefaultValuePoliciesBase<FlowDirectionElement, MaterialElement>;
                using MaterialOutputPolicies = OutputPoliciesT<PoliciesBase, 0>;
                using FlowDirectionInputPolicies = InputPoliciesT<PoliciesBase, 0>;
                using MaterialInputPolicies = InputPoliciesT<PoliciesBase, 1>;

                DefaultValuePolicies():

                    PoliciesBase{
                            DomainPolicyT<PoliciesBase>{},
                            MaterialOutputPolicies{},
                            FlowDirectionInputPolicies{},
                            MaterialInputPolicies{FillHaloWithConstantValue<MaterialElement>{0}}
                        }

                {
                }

        };


    }  // namespace policy::flow_accumulation
}  // namespace lue
