#pragma once
#include "lue/framework/algorithm/definition/focal_operation.hpp"
#include "lue/framework/algorithm/downstream.hpp"
#include "lue/framework/algorithm/flow_direction.hpp"
#include "lue/framework/algorithm/routing_operation_export.hpp"
#include "lue/framework/algorithm/serialize/kernel.hpp"


namespace lue {
    namespace detail {

        template<typename FlowDirectionElement, typename MaterialElement>
        class Downstream
        {

            public:

                static constexpr char const* name{"downstream"};

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
                    if (indp1.is_no_data(flow_direction_window[1, 1]) ||
                        indp2.is_no_data(material_window[1, 1]))
                    {
                        ondp.mark_no_data(material);
                    }
                    else
                    {
                        // If a downstream material contains no-data,
                        // then the result will be marked as no-data
                        if (flow_direction_window[1, 1] == north_west<FlowDirectionElement>)
                        {
                            if (indp2.is_no_data(material_window[0, 0]))
                            {
                                ondp.mark_no_data(material);
                            }
                            else
                            {
                                material = material_window[0, 0];
                            }
                        }
                        else if (flow_direction_window[1, 1] == north<FlowDirectionElement>)
                        {
                            if (indp2.is_no_data(material_window[0, 1]))
                            {
                                ondp.mark_no_data(material);
                            }
                            else
                            {
                                material = material_window[0, 1];
                            }
                        }
                        else if (flow_direction_window[1, 1] == north_east<FlowDirectionElement>)
                        {
                            if (indp2.is_no_data(material_window[0, 2]))
                            {
                                ondp.mark_no_data(material);
                            }
                            else
                            {
                                material = material_window[0, 2];
                            }
                        }
                        else if (flow_direction_window[1, 1] == west<FlowDirectionElement>)
                        {
                            if (indp2.is_no_data(material_window[1, 0]))
                            {
                                ondp.mark_no_data(material);
                            }
                            else
                            {
                                material = material_window[1, 0];
                            }
                        }
                        else if (flow_direction_window[1, 1] == east<FlowDirectionElement>)
                        {
                            if (indp2.is_no_data(material_window[1, 2]))
                            {
                                ondp.mark_no_data(material);
                            }
                            else
                            {
                                material = material_window[1, 2];
                            }
                        }
                        else if (flow_direction_window[1, 1] == south_west<FlowDirectionElement>)
                        {
                            if (indp2.is_no_data(material_window[2, 0]))
                            {
                                ondp.mark_no_data(material);
                            }
                            else
                            {
                                material = material_window[2, 0];
                            }
                        }
                        else if (flow_direction_window[1, 1] == south<FlowDirectionElement>)
                        {
                            if (indp2.is_no_data(material_window[2, 1]))
                            {
                                ondp.mark_no_data(material);
                            }
                            else
                            {
                                material = material_window[2, 1];
                            }
                        }
                        else if (flow_direction_window[1, 1] == south_east<FlowDirectionElement>)
                        {
                            if (indp2.is_no_data(material_window[2, 2]))
                            {
                                ondp.mark_no_data(material);
                            }
                            else
                            {
                                material = material_window[2, 2];
                            }
                        }
                        else
                        {
                            lue_hpx_assert((flow_direction_window[1, 1] == sink<FlowDirectionElement>));
                            material = material_window[1, 1];
                        }
                    }

                    return material;
                }
        };

    }  // namespace detail


    template<typename Policies, typename FlowDirectionElement, typename MaterialElement, Rank rank>
    PartitionedArray<MaterialElement, rank> downstream(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
        PartitionedArray<MaterialElement, rank> const& material)
    {
        using Functor = detail::Downstream<FlowDirectionElement, MaterialElement>;

        // Only used for its radius. Weights are not used.
        auto kernel{box_kernel<bool, 2>(1, true)};

        return focal_operation(policies, flow_direction, material, std::move(kernel), Functor{});
    }

}  // namespace lue


#define LUE_INSTANTIATE_DOWNSTREAM(Policies, FlowDirectionElement, MaterialElement)                          \
                                                                                                             \
    template LUE_ROUTING_OPERATION_EXPORT PartitionedArray<MaterialElement, 2>                               \
    downstream<ArgumentType<void(Policies)>, FlowDirectionElement, MaterialElement, 2>(                      \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<FlowDirectionElement, 2> const&,                                                    \
        PartitionedArray<MaterialElement, 2> const&);
