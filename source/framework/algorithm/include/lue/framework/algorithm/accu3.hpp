#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array.hpp"


namespace lue {
    namespace policy::accu3 {

        template<
            typename FlowDirectionElement,
            typename MaterialElement>
        using DefaultPoliciesBase = Policies<
            AllValuesWithinDomain<FlowDirectionElement, MaterialElement>,
            OutputsPolicies<
                OutputPolicies<DontMarkNoData<MaterialElement>>>,
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
                using FluxOutputPolicies = OutputPoliciesT<PoliciesBase, 0>;
                using FlowDirectionInputPolicies = InputPoliciesT<PoliciesBase, 0>;
                using MaterialInputPolicies = InputPoliciesT<PoliciesBase, 1>;

                DefaultPolicies():

                    PoliciesBase{
                            DomainPolicyT<PoliciesBase>{},
                            FluxOutputPolicies{},
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


    template<
        typename Policies,
        typename FlowDirectionElement,
        typename MaterialElement,
        Rank rank>
    PartitionedArray<MaterialElement, rank> accu3(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
        PartitionedArray<MaterialElement, rank> const& external_inflow);


    template<
        typename FlowDirectionElement,
        typename MaterialElement,
        Rank rank>
    PartitionedArray<MaterialElement, rank> accu3(
        PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
        PartitionedArray<MaterialElement, rank> const& external_inflow)
    {
        using Policies = policy::accu3::DefaultPolicies<FlowDirectionElement, MaterialElement>;

        return accu3(Policies{}, flow_direction, external_inflow);
    }

}  // namespace lue
