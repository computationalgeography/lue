#pragma once
#include "lue/framework/algorithm/component/server/decreasing_order_zone.hpp"


namespace lue {

    template<typename Zone, typename Value, Rank rank>
    class DecreasingOrderZone:
        public hpx::components::client_base<
            DecreasingOrderZone<Zone, Value, rank>,
            server::DecreasingOrderZone<Zone, Value, rank>>

    {

        public:

            using Server = server::DecreasingOrderZone<Zone, Value, rank>;

            using ZonePartition = typename Server::ZonePartition;

            using ValuePartition = typename Server::ValuePartition;

            using RouteID = typename Server::RouteID;

            using Base = hpx::components::client_base<DecreasingOrderZone<Zone, Value, rank>, Server>;


            DecreasingOrderZone():

                Base{}

            {
            }


            DecreasingOrderZone(hpx::id_type component_id):

                Base{component_id}

            {
            }


            DecreasingOrderZone(hpx::future<hpx::id_type>&& component_id):

                Base{std::move(component_id)}

            {
            }


            DecreasingOrderZone(DecreasingOrderZone const&) = default;

            DecreasingOrderZone(DecreasingOrderZone&&) = default;

            ~DecreasingOrderZone() = default;

            DecreasingOrderZone& operator=(DecreasingOrderZone const&) = default;

            DecreasingOrderZone& operator=(DecreasingOrderZone&&) = default;


            hpx::future<std::map<Zone, Value>> sort_values(
                ZonePartition const& zone_partition, ValuePartition const& value_partition) const
            {
                lue_hpx_assert(this->is_ready());
                lue_hpx_assert(this->get_id());

                typename Server::SortValuesAction action;

                return hpx::async(action, this->get_id(), zone_partition, value_partition);
            }

            hpx::future<RouteID> record_route(int maxima, Count const max_length) const
            {
                lue_hpx_assert(this->is_ready());
                lue_hpx_assert(this->get_id());

                typename Server::RecordRouteAction action;

                return hpx::async(action, this->get_id(), maxima, max_length);
            }
    };

}  // namespace lue


#define LUE_REGISTER_DECREASING_ORDER_ZONE_ACTIONS(Zone, Value, rank)                                        \
                                                                                                             \
    namespace lue::detail {                                                                                  \
                                                                                                             \
        using DecreasingOrderZoneServerType_##Zone##_##Value##_##rank =                                      \
            hpx::components::component<DecreasingOrderZone_##Zone##_##Value##_##rank>;                       \
    }                                                                                                        \
                                                                                                             \
    HPX_REGISTER_COMPONENT(                                                                                  \
        lue::detail::DecreasingOrderZoneServerType_##Zone##_##Value##_##rank,                                \
        DecreasingOrderZoneServer_##Zone##_##Value##_##rank)                                                 \
                                                                                                             \
    HPX_REGISTER_ACTION(                                                                                     \
        lue::detail::DecreasingOrderZone_##Zone##_##Value##_##rank::SortValuesAction,                        \
        DecreasingOrderZone_##Zone##_##Value##_##rank##_SortValuesAction)                                    \
                                                                                                             \
    HPX_REGISTER_ACTION(                                                                                     \
        lue::detail::DecreasingOrderZone_##Zone##_##Value##_##rank::RecordRouteAction,                       \
        DecreasingOrderZone_##Zone##_##Value##_##rank##_RecordRouteAction)
