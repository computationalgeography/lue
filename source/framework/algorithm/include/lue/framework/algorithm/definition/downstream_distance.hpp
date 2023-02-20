#pragma once
#include "lue/framework/algorithm/definition/unary_local_operation.hpp"
#include "lue/framework/algorithm/downstream_distance.hpp"
#include "lue/framework/algorithm/flow_direction.hpp"
#include "lue/framework/algorithm/routing_operation_export.hpp"
#include "lue/macro.hpp"
#include <hpx/serialization.hpp>
#include <cmath>


namespace lue {
    namespace detail {

        template<typename FlowDirectionElement, typename DistanceElement>
        class DownstreamDistance
        {

            public:

                static_assert(std::is_floating_point_v<DistanceElement>);

                using OutputElement = DistanceElement;


                DownstreamDistance() = default;


                DownstreamDistance(DistanceElement const& cell_size):

                    _cell_size{cell_size},
                    _cell_size_diagonal{std::sqrt(DistanceElement{2}) * cell_size}

                {
                }


                DistanceElement operator()(FlowDirectionElement const flow_direction) const noexcept
                {
                    DistanceElement distance{0};

                    if (flow_direction == north_west<FlowDirectionElement> ||
                        flow_direction == north_east<FlowDirectionElement> ||
                        flow_direction == south_west<FlowDirectionElement> ||
                        flow_direction == south_east<FlowDirectionElement>)
                    {
                        distance = _cell_size_diagonal;
                    }
                    else if (
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
                void serialize(Archive& archive, [[maybe_unused]] unsigned int const version)
                {
                    archive& _cell_size& _cell_size_diagonal;
                }


                DistanceElement _cell_size;

                DistanceElement _cell_size_diagonal;
        };

    }  // namespace detail


    template<typename Policies, typename FlowDirectionElement, typename DistanceElement, Rank rank>
    PartitionedArray<DistanceElement, rank> downstream_distance(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
        DistanceElement const cell_size)
    {
        static_assert(rank == 2);

        using Functor = detail::DownstreamDistance<FlowDirectionElement, DistanceElement>;

        return unary_local_operation(policies, flow_direction, Functor{cell_size});
    }

}  // namespace lue


#define LUE_INSTANTIATE_DOWNSTREAM_DISTANCE(Policies, FlowDirectionElement, DistanceElement)                 \
                                                                                                             \
    template LUE_ROUTING_OPERATION_EXPORT PartitionedArray<DistanceElement, 2>                               \
    downstream_distance<ArgumentType<void(Policies)>, FlowDirectionElement, DistanceElement, 2>(             \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<FlowDirectionElement, 2> const&,                                                    \
        DistanceElement const);
