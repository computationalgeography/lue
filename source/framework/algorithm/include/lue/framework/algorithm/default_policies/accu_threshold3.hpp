#pragma once
#include "lue/framework/algorithm/accu_threshold3.hpp"


namespace lue {
    namespace policy::accu_threshold3 {

        template<
            typename FlowDirectionElement,
            typename MaterialElement>
        using DefaultPoliciesBase = Policies<
            AllValuesWithinDomain<MaterialElement, MaterialElement>,
            OutputsPolicies<
                OutputPolicies<DontMarkNoData<MaterialElement>>,
                OutputPolicies<DontMarkNoData<MaterialElement>>>,
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

    }  // namespace policy::accu_threshold3


    namespace default_policies {

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
                policy::accu_threshold3::DefaultPolicies<FlowDirectionElement, MaterialElement>;

            return accu_threshold3(Policies{}, flow_direction, external_inflow, threshold);
        }

    }  // namespace default_policies
}  // namespace lue
