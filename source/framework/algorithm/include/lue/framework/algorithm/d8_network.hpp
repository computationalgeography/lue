#pragma once
#include "lue/framework/algorithm/flow_direction.hpp"
#include "lue/framework/algorithm/focal_operation.hpp"
#include "lue/framework/algorithm/kernel.hpp"
#include "lue/framework/algorithm/policy/all_values_within_domain.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"
#include <type_traits>


namespace lue {
    namespace detail {

        template<
            typename FlowDirectionElement,
            typename ElevationElement>
        class D8Network
        {

        public:

            using OutputElement = FlowDirectionElement;

            D8Network()=default;

            template<
                typename Kernel,
                typename OutputPolicies,
                typename InputPolicies,
                typename Subspan>
            FlowDirectionElement operator()(
                [[maybe_unused]] Kernel const& kernel,
                [[maybe_unused]] OutputPolicies const& output_policies,
                [[maybe_unused]] InputPolicies const& input_policies,
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

                ElevationElement drop{0};
                FlowDirectionElement direction{sink<FlowDirectionElement>};

                ElevationElement other_drop;

                // First try the nearest cells -----------------------------------------
                // North
                other_drop = window(1, 1) - window(0, 1);
                if(other_drop > drop)
                {
                    drop = other_drop;
                    direction = north<FlowDirectionElement>;
                }

                // West
                other_drop = window(1, 1) - window(1, 0);
                if(other_drop > drop)
                {
                    drop = other_drop;
                    direction = west<FlowDirectionElement>;
                }

                // East
                other_drop = window(1, 1) - window(1, 2);
                if(other_drop > drop)
                {
                    drop = other_drop;
                    direction = east<FlowDirectionElement>;
                }

                // South
                other_drop = window(1, 1) - window(2, 1);
                if(other_drop > drop)
                {
                    drop = other_drop;
                    direction = south<FlowDirectionElement>;
                }

                // Move on the diagonal cells ------------------------------------------
                // North-west
                other_drop = window(1, 1) - window(0, 0);
                if(other_drop > drop)
                {
                    drop = other_drop;
                    direction = north_west<FlowDirectionElement>;
                }

                // North-east
                other_drop = window(1, 1) - window(0, 2);
                if(other_drop > drop)
                {
                    drop = other_drop;
                    direction = north_east<FlowDirectionElement>;
                }

                // South-west
                other_drop = window(1, 1) - window(2, 0);
                if(other_drop > drop)
                {
                    drop = other_drop;
                    direction = south_west<FlowDirectionElement>;
                }

                // South-east
                other_drop = window(1, 1) - window(2, 2);
                if(other_drop > drop)
                {
                    drop = other_drop;
                    direction = south_east<FlowDirectionElement>;
                }

                return direction;
            }

        };

    }  // namespace detail


    namespace policy::d8 {

        template<
            typename OutputElement,
            typename InputElement>
        using DefaultPolicies = policy::DefaultSpatialOperationPolicies<
            AllValuesWithinDomain<InputElement>,
            OutputElements<OutputElement>,
            InputElements<InputElement>>;

    }  // namespace policy::d8


    template<
        typename FlowDirectionElement,
        typename ElevationElement,
        Rank rank>
    PartitionedArray<FlowDirectionElement, rank> d8_network(
        PartitionedArray<ElevationElement, rank> const& elevation)
    {
        using Weight = bool;
        using Functor = detail::D8Network<FlowDirectionElement, ElevationElement>;
        using Policies = policy::d8::DefaultPolicies<FlowDirectionElement, ElevationElement>;

        ElevationElement const fill_value{std::numeric_limits<ElevationElement>::min()};
        Kernel<Weight, rank> kernel{box_kernel<Weight, rank>(1, true)};

        return focal_operation(Policies{fill_value}, elevation, kernel, Functor{});
    }

}  // namespace lue
