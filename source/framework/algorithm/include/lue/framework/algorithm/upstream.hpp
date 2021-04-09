#pragma once
#include "lue/framework/algorithm/flow_direction.hpp"
#include "lue/framework/algorithm/focal_operation.hpp"
#include "lue/framework/algorithm/serialize/kernel.hpp"
#include "lue/framework/algorithm/policy/all_values_within_domain.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"


namespace lue {
    namespace detail {

        template<
            typename FlowDirectionElement,
            typename MaterialElement>
        class Upstream
        {

            public:

                using OutputElement = MaterialElement;


                template<
                    typename Kernel,
                    typename OutputPolicies,
                    typename InputPolicies1,
                    typename InputPolicies2,
                    typename Subspan1,
                    typename Subspan2>
                OutputElement operator()(
                    [[maybe_unused]] Kernel const& kernel,
                    OutputPolicies const& output_policies,
                    InputPolicies1 const& input_policies1,
                    InputPolicies2 const& input_policies2,
                    Subspan1 const& flow_direction_window,
                    Subspan2 const& material_window) const
                {
                    static_assert(rank<Kernel> == 2);
                    lue_hpx_assert(kernel.radius() == 1);

                    // Iterate over all neighbours and for each upstream
                    // cell, draining into the focal cell, grab the material

                    auto indp1 = input_policies1.input_no_data_policy();
                    auto indp2 = input_policies2.input_no_data_policy();
                    auto ondp = output_policies.output_no_data_policy();

                    OutputElement material{0};

                    // If input flow direction or material contain a
                    // no-data in the focal cell, then the result will be
                    // marked as no-data
                    if(indp1.is_no_data(flow_direction_window(1, 1)) ||
                        indp2.is_no_data(material_window(1, 1)))
                    {
                        ondp.mark_no_data(material);
                        return material;
                    }

                    // If an upstream material contains no-data,
                    // then the result will be marked as no-data

                    // NOTE: We are not testing upstream flow-direction
                    //     for no-data-ness. This should not be a problem,
                    //     but in theory it can be, depending on how
                    //     the policies and data are organized. For
                    //     example, if the data is only stored for
                    //     cells with valid values accessing values for
                    //     no-data cells may result in undefined
                    //     behaviour. But organizing things like that
                    //     may never happen.

                    if(flow_direction_window(0, 0) == south_east<FlowDirectionElement>)
                    {
                        if(indp2.is_no_data(material_window(0, 0)))
                        {
                            ondp.mark_no_data(material);
                            return material;
                        }

                        material += material_window(0, 0);
                    }

                    if(flow_direction_window(0, 1) == south<FlowDirectionElement>)
                    {
                        if(indp2.is_no_data(material_window(0, 1)))
                        {
                            ondp.mark_no_data(material);
                            return material;
                        }

                        material += material_window(0, 1);
                    }

                    if(flow_direction_window(0, 2) == south_west<FlowDirectionElement>)
                    {
                        if(indp2.is_no_data(material_window(0, 2)))
                        {
                            ondp.mark_no_data(material);
                            return material;
                        }

                        material += material_window(0, 2);
                    }

                    if(flow_direction_window(1, 0) == east<FlowDirectionElement>)
                    {
                        if(indp2.is_no_data(material_window(1, 0)))
                        {
                            ondp.mark_no_data(material);
                            return material;
                        }

                        material += material_window(1, 0);
                    }

                    if(flow_direction_window(1, 2) == west<FlowDirectionElement>)
                    {
                        if(indp2.is_no_data(material_window(1, 2)))
                        {
                            ondp.mark_no_data(material);
                            return material;
                        }

                        material += material_window(1, 2);
                    }

                    if(flow_direction_window(2, 0) == north_east<FlowDirectionElement>)
                    {
                        if(indp2.is_no_data(material_window(2, 0)))
                        {
                            ondp.mark_no_data(material);
                            return material;
                        }

                        material += material_window(2, 0);
                    }

                    if(flow_direction_window(2, 1) == north<FlowDirectionElement>)
                    {
                        if(indp2.is_no_data(material_window(2, 1)))
                        {
                            ondp.mark_no_data(material);
                            return material;
                        }

                        material += material_window(2, 1);
                    }

                    if(flow_direction_window(2, 2) == north_west<FlowDirectionElement>)
                    {
                        if(indp2.is_no_data(material_window(2, 2)))
                        {
                            ondp.mark_no_data(material);
                            return material;
                        }

                        material += material_window(2, 2);
                    }

                    return material;
                }

        };

    }  // namespace detail


    namespace policy::flow_accumulation {

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
                OutputPolicies<DefaultOutputNoDataPolicy<MaterialElement>>>,
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


    namespace policy::upstream {

        template<
            typename FlowDirectionElement,
            typename MaterialElement>
        using DefaultPolicies =
            flow_accumulation::DefaultPolicies<FlowDirectionElement, MaterialElement>;

        template<
            typename FlowDirectionElement,
            typename MaterialElement>
        using DefaultValuePolicies =
            flow_accumulation::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

    }  // namespace upstream::policy


    template<
        typename Policies,
        typename FlowDirectionElement,
        typename MaterialElement,
        Rank rank>
    PartitionedArray<MaterialElement, rank> upstream(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
        PartitionedArray<MaterialElement, rank> const& material)
    {
        using Functor = detail::Upstream<FlowDirectionElement, MaterialElement>;

        // Only used for its radius. Weights are not used.
        auto kernel{box_kernel<bool, 2>(1, true)};

        return focal_operation(policies, flow_direction, material, std::move(kernel), Functor{});
    }


    template<
        typename FlowDirectionElement,
        typename MaterialElement,
        Rank rank>
    PartitionedArray<MaterialElement, rank> upstream(
        PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
        PartitionedArray<MaterialElement, rank> const& material)
    {
        using Policies = policy::upstream::DefaultPolicies<FlowDirectionElement, MaterialElement>;

        return upstream(Policies{}, flow_direction, material);
    }

}  // namespace lue
