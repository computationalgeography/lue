#pragma once
#include "lue/framework/algorithm/d8_flow_direction.hpp"
#include "lue/framework/algorithm/definition/focal_operation.hpp"
#include "lue/framework/algorithm/flow_direction.hpp"
#include "lue/framework/algorithm/kernel.hpp"
#include "lue/framework/algorithm/routing_operation_export.hpp"
#include "lue/macro.hpp"


namespace lue {
    namespace detail {

        template<std::integral FlowDirectionElement, Arithmetic ElevationElement>
        class D8FlowDirection
        {

            public:

                static constexpr char const* name{"d8_flow_direction"};

                using OutputElement = FlowDirectionElement;


                template<typename Kernel, typename OutputPolicies, typename InputPolicies, typename Subspan>
                auto operator()(
                    [[maybe_unused]] Kernel const& kernel,
                    OutputPolicies const& output_policies,
                    InputPolicies const& input_policies,
                    Subspan const& window) const -> FlowDirectionElement
                {
                    // We are assuming a 3x3 kernel, containing only true weights
                    static_assert(rank<Kernel> == 2);
                    lue_hpx_assert(kernel.size() == 3);
                    lue_hpx_assert(
                        std::all_of(
                            kernel.begin(), kernel.end(), [](auto const weight) { return bool{weight}; }));

                    lue_hpx_assert(window.extent(0) == kernel.size());
                    lue_hpx_assert(window.extent(1) == kernel.size());

                    auto const& indp{input_policies.input_no_data_policy()};
                    auto const& ondp{output_policies.output_no_data_policy()};

                    FlowDirectionElement direction{};

                    if (indp.is_no_data(window[1, 1]))
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
                        if (!indp.is_no_data(window[0, 1]))
                        {
                            other_drop = window[1, 1] - window[0, 1];
                            if (other_drop > drop)
                            {
                                drop = other_drop;
                                direction = north<FlowDirectionElement>;
                            }
                        }

                        // West
                        if (!indp.is_no_data(window[1, 0]))
                        {
                            other_drop = window[1, 1] - window[1, 0];
                            if (other_drop > drop)
                            {
                                drop = other_drop;
                                direction = west<FlowDirectionElement>;
                            }
                        }

                        // East
                        if (!indp.is_no_data(window[1, 2]))
                        {
                            other_drop = window[1, 1] - window[1, 2];
                            if (other_drop > drop)
                            {
                                drop = other_drop;
                                direction = east<FlowDirectionElement>;
                            }
                        }

                        // South
                        if (!indp.is_no_data(window[2, 1]))
                        {
                            other_drop = window[1, 1] - window[2, 1];
                            if (other_drop > drop)
                            {
                                drop = other_drop;
                                direction = south<FlowDirectionElement>;
                            }
                        }

                        // Move on the diagonal cells ------------------------------------------
                        // North-west
                        if (!indp.is_no_data(window[0, 0]))
                        {
                            other_drop = window[1, 1] - window[0, 0];
                            if (other_drop > drop)
                            {
                                drop = other_drop;
                                direction = north_west<FlowDirectionElement>;
                            }
                        }

                        // North-east
                        if (!indp.is_no_data(window[0, 2]))
                        {
                            other_drop = window[1, 1] - window[0, 2];
                            if (other_drop > drop)
                            {
                                drop = other_drop;
                                direction = north_east<FlowDirectionElement>;
                            }
                        }

                        // South-west
                        if (!indp.is_no_data(window[2, 0]))
                        {
                            other_drop = window[1, 1] - window[2, 0];
                            if (other_drop > drop)
                            {
                                drop = other_drop;
                                direction = south_west<FlowDirectionElement>;
                            }
                        }

                        // South-east
                        if (!indp.is_no_data(window[2, 2]))
                        {
                            other_drop = window[1, 1] - window[2, 2];
                            if (other_drop > drop)
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


    /*!
      @brief      Determine the D8 flow direction in each cell for the array passed in
      @ingroup    routing_operation
    */
    template<typename Policies>
        requires Arithmetic<policy::InputElementT<Policies, 0>> &&
                 std::integral<policy::OutputElementT<Policies, 0>>
    auto d8_flow_direction(
        Policies const& policies, PartitionedArray<policy::InputElementT<Policies, 0>, 2> const& elevation)
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, 2>
    {
        using ElevationElement = policy::InputElementT<Policies, 0>;
        using FlowDirectionElement = policy::OutputElementT<Policies, 0>;
        using Weight = bool;
        using Functor = detail::D8FlowDirection<FlowDirectionElement, ElevationElement>;

        Kernel<Weight, 2> kernel{box_kernel<Weight, 2>(1, true)};

        return focal_operation(policies, elevation, kernel, Functor{});
    }

}  // namespace lue


#define LUE_INSTANTIATE_D8_FLOW_DIRECTION(Policies)                                                          \
                                                                                                             \
    template LUE_ROUTING_OPERATION_EXPORT auto d8_flow_direction<ArgumentType<void(Policies)>>(              \
        ArgumentType<void(Policies)> const&, PartitionedArray<policy::InputElementT<Policies, 0>, 2> const&) \
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, 2>;
