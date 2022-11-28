#pragma once
#include "lue/framework/algorithm/d8_flow_direction.hpp"
#include "lue/framework/algorithm/flow_direction.hpp"
#include "lue/framework/algorithm/routing_operation_export.hpp"
#include "lue/framework/algorithm/definition/focal_operation.hpp"
#include "lue/framework/algorithm/serialize/kernel.hpp"
#include "lue/macro.hpp"


namespace lue {
    namespace detail {

        template<
            typename FlowDirectionElement,
            typename ElevationElement>
        class D8FlowDirection
        {

            public:

                using OutputElement = FlowDirectionElement;


                D8FlowDirection()=default;


                template<
                    typename Kernel,
                    typename OutputPolicies,
                    typename InputPolicies,
                    typename Subspan>
                FlowDirectionElement operator()(
                    [[maybe_unused]] Kernel const& kernel,
                    OutputPolicies const& output_policies,
                    InputPolicies const& input_policies,
                    Subspan const& window) const
                {
                    using Weight = ElementT<Kernel>;

                    // TODO Add traits to grab typename of elements in Subspan
                    // static_assert(std::is_same_v<ElementT<Subspan>, ElevationElement>);
                    static_assert(std::is_convertible_v<Weight, bool>);

                    // We are assuming a 3x3 kernel, containing only true weights
                    static_assert(rank<Kernel> == 2);
                    lue_hpx_assert(kernel.size() == 3);
                    lue_hpx_assert(std::all_of(kernel.begin(), kernel.end(), [](auto w) { return bool{w}; }));

                    lue_hpx_assert(window.extent(0) == kernel.size());
                    lue_hpx_assert(window.extent(1) == kernel.size());

                    auto const& indp{input_policies.input_no_data_policy()};
                    auto const& ondp{output_policies.output_no_data_policy()};

                    FlowDirectionElement direction;

                    if(indp.is_no_data(window(1, 1)))
                    {
                        ondp.mark_no_data(direction);
                    }
                    else
                    {
                        direction = sink<FlowDirectionElement>;

                        ElevationElement drop{0};
                        ElevationElement other_drop;

                        // First try the nearest cells -----------------------------------------
                        // North
                        if(!indp.is_no_data(window(0, 1)))
                        {
                            other_drop = window(1, 1) - window(0, 1);
                            if(other_drop > drop)
                            {
                                drop = other_drop;
                                direction = north<FlowDirectionElement>;
                            }
                        }

                        // West
                        if(!indp.is_no_data(window(1, 0)))
                        {
                            other_drop = window(1, 1) - window(1, 0);
                            if(other_drop > drop)
                            {
                                drop = other_drop;
                                direction = west<FlowDirectionElement>;
                            }
                        }

                        // East
                        if(!indp.is_no_data(window(1, 2)))
                        {
                            other_drop = window(1, 1) - window(1, 2);
                            if(other_drop > drop)
                            {
                                drop = other_drop;
                                direction = east<FlowDirectionElement>;
                            }
                        }

                        // South
                        if(!indp.is_no_data(window(2, 1)))
                        {
                            other_drop = window(1, 1) - window(2, 1);
                            if(other_drop > drop)
                            {
                                drop = other_drop;
                                direction = south<FlowDirectionElement>;
                            }
                        }

                        // Move on the diagonal cells ------------------------------------------
                        // North-west
                        if(!indp.is_no_data(window(0, 0)))
                        {
                            other_drop = window(1, 1) - window(0, 0);
                            if(other_drop > drop)
                            {
                                drop = other_drop;
                                direction = north_west<FlowDirectionElement>;
                            }
                        }

                        // North-east
                        if(!indp.is_no_data(window(0, 2)))
                        {
                            other_drop = window(1, 1) - window(0, 2);
                            if(other_drop > drop)
                            {
                                drop = other_drop;
                                direction = north_east<FlowDirectionElement>;
                            }
                        }

                        // South-west
                        if(!indp.is_no_data(window(2, 0)))
                        {
                            other_drop = window(1, 1) - window(2, 0);
                            if(other_drop > drop)
                            {
                                drop = other_drop;
                                direction = south_west<FlowDirectionElement>;
                            }
                        }

                        // South-east
                        if(!indp.is_no_data(window(2, 2)))
                        {
                            other_drop = window(1, 1) - window(2, 2);
                            if(other_drop > drop)
                            {
                                drop = other_drop;
                                direction = south_east<FlowDirectionElement>;
                            }
                        }
                    }

                    return direction;
                }

        };

    }  // namespace detail


    template<
        typename FlowDirectionElement,
        typename Policies,
        typename ElevationElement,
        Rank rank>
    PartitionedArray<FlowDirectionElement, rank> d8_flow_direction(
        Policies const& policies,
        PartitionedArray<ElevationElement, rank> const& elevation)
    {
        using Weight = bool;
        using Functor = detail::D8FlowDirection<FlowDirectionElement, ElevationElement>;

        Kernel<Weight, rank> kernel{box_kernel<Weight, rank>(1, true)};

        return focal_operation(policies, elevation, kernel, Functor{});
    }

}  // namespace lue


#define LUE_INSTANTIATE_D8_FLOW_DIRECTION(                                             \
    Policies, FlowDirectionElement, ElevationElement)                                  \
                                                                                       \
    template LUE_ROUTING_OPERATION_EXPORT                                              \
    PartitionedArray<FlowDirectionElement, 2> d8_flow_direction<                       \
            FlowDirectionElement, ArgumentType<void(Policies)>, ElevationElement, 2>(  \
        ArgumentType<void(Policies)> const&,                                           \
        PartitionedArray<ElevationElement, 2> const&);
