#pragma once
#include "lue/framework/algorithm/flow_direction.hpp"
#include "lue/framework/algorithm/unary_local_operation.hpp"
#include "lue/framework/algorithm/policy.hpp"
#include <hpx/serialization.hpp>
#include <cmath>


namespace lue {
    namespace detail {

        template<
            typename FlowDirectionElement,
            typename DistanceElement>
        class DownstreamDistance
        {

            public:

                static_assert(std::is_floating_point_v<DistanceElement>);

                using OutputElement = DistanceElement;


                DownstreamDistance()=default;


                DownstreamDistance(
                    DistanceElement const& cell_size):

                    _cell_size{cell_size},
                    _cell_size_diagonal{std::sqrt(DistanceElement{2}) * cell_size}

                {
                }


                DistanceElement operator()(
                    FlowDirectionElement const flow_direction) const noexcept
                {
                    DistanceElement distance{0};

                    if(
                        flow_direction == north_west<FlowDirectionElement> ||
                        flow_direction == north_east<FlowDirectionElement> ||
                        flow_direction == south_west<FlowDirectionElement> ||
                        flow_direction == south_east<FlowDirectionElement>)
                    {
                        distance = _cell_size_diagonal;
                    }
                    else if(
                        flow_direction == north<FlowDirectionElement> ||
                        flow_direction == west<FlowDirectionElement> ||
                        flow_direction == east<FlowDirectionElement> ||
                        flow_direction == south<FlowDirectionElement>)
                    {
                        distance = _cell_size;
                    }

                    return distance;
                }


            private:

                friend class hpx::serialization::access;


                template<typename Archive>
                void serialize(
                    Archive& archive,
                    [[maybe_unused]] unsigned int const version)
                {
                    archive & _cell_size & _cell_size_diagonal;
                }


                DistanceElement _cell_size;

                DistanceElement _cell_size_diagonal;

        };

    }  // namespace detail


    namespace policy::downstream_distance {

        template<
            typename FlowDirectionElement,
            typename DistanceElement>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<FlowDirectionElement>,
            OutputElements<DistanceElement>,
            InputElements<FlowDirectionElement>>;

        template<
            typename FlowDirectionElement,
            typename DistanceElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<FlowDirectionElement>,
            OutputElements<DistanceElement>,
            InputElements<FlowDirectionElement>>;

    }  // namespace downstream_distance::policy


    template<
        typename Policies,
        typename FlowDirectionElement,
        typename DistanceElement,
        Rank rank>
    PartitionedArray<DistanceElement, rank> downstream_distance(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
        DistanceElement const cell_size)
    {
        static_assert(rank == 2);

        using Functor = detail::DownstreamDistance<FlowDirectionElement, DistanceElement>;

        return unary_local_operation(policies, flow_direction, Functor{cell_size});
    }


    template<
        typename FlowDirectionElement,
        typename DistanceElement,
        Rank rank>
    PartitionedArray<DistanceElement, rank> downstream_distance(
        PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
        DistanceElement const cell_size)
    {
        using Policies = policy::downstream_distance::DefaultPolicies<FlowDirectionElement, DistanceElement>;

        return downstream_distance(Policies{}, flow_direction, cell_size);
    }

}  // namespace lue
