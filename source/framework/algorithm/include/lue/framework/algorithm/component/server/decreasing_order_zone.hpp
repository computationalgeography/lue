#pragma once
#include "lue/framework/partitioned_array/array_partition.hpp"
#include <hpx/include/components.hpp>
#include <map>


namespace lue::server {

    template<typename Zone, typename Value, Rank rank>
    class DecreasingOrderZone: public hpx::components::component_base<DecreasingOrderZone<Zone, Value, rank>>
    {

        private:

            using Base = hpx::components::component_base<DecreasingOrderZone<Zone, Value, rank>>;

            using ZonePartition = lue::ArrayPartition<Zone, rank>;

            using ValuePartition = lue::ArrayPartition<Value, rank>;

            using RouteID = Index;

        public:

            DecreasingOrderZone():

                Base{}

            {
            }


            DecreasingOrderZone(DecreasingOrderZone const&) = default;

            DecreasingOrderZone(DecreasingOrderZone&&) = default;

            ~DecreasingOrderZone() = default;

            DecreasingOrderZone& operator=(DecreasingOrderZone const&) = default;

            DecreasingOrderZone& operator=(DecreasingOrderZone&&) = default;


            std::map<Zone, Value> sort_values(
                ZonePartition const& zone_partition, ValuePartition const& value_partition)
            {
                //     - sort_values(zone, value) → Fill collections, return maximum per zone

                // TODO

                std::map<Zone, Value> result{};

                return result;
            }


            RouteID record_route(int maxima, Count const max_length)
            {
                //     - record_route(maxima, max_length)

                // TODO

                RouteID route_id{};

                return route_id;
            }


            HPX_DEFINE_COMPONENT_ACTION(DecreasingOrderZone, sort_values, SortValuesAction);

            HPX_DEFINE_COMPONENT_ACTION(DecreasingOrderZone, record_route, RecordRouteAction);

        private:
    };


}  // namespace lue::server


#define LUE_REGISTER_DECREASING_ORDER_ZONE_ACTION_DECLARATIONS(Zone, Value, rank)                            \
                                                                                                             \
    namespace lue::detail {                                                                                  \
                                                                                                             \
        using DecreasingOrderZone_##Zone##_##Value##_##rank =                                                \
            server::DecreasingOrderZone<Zone, Value, rank>;                                                  \
    }                                                                                                        \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        lue::detail::DecreasingOrderZone_##Zone##_##Value##_##rank::SortValuesAction,                        \
        DecreasingOrderZone_##Zone##_##Value##_##rank##_SortValuesAction)                                    \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        lue::detail::DecreasingOrderZone_##Zone##_##Value##_##rank::RecordRouteAction,                       \
        DecreasingOrderZone_##Zone##_##Value##_##rank##_RecordRouteAction)
