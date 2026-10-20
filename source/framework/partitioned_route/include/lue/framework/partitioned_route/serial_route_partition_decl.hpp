#pragma once
#include "lue/framework/partitioned_route/export.hpp"
#include "lue/framework/partitioned_route/server/serial_route_partition_decl.hpp"


namespace lue {

    /*!
        @brief      Class for serial route partition component client instances

        Each instance is associated with a single serial route partition component
        server instance. Multiple instances can be associated with a single server instance.
    */
    template<typename RouteID, Rank rank>
    class LUE_PARTITIONED_ROUTE_EXPORT SerialRoutePartition:
        public hpx::components::
            client_base<SerialRoutePartition<RouteID, rank>, server::SerialRoutePartition<RouteID, rank>>

    {

        public:

            using Server = server::SerialRoutePartition<RouteID, rank>;

            using Base = hpx::components::client_base<SerialRoutePartition<RouteID, rank>, Server>;

            using RouteFragment = typename Server::RouteFragment;

            using RouteFragments = typename Server::RouteFragments;

            using RouteFragmentLocation = typename RouteFragment::Location;

            using Offset = typename Server::Offset;

            using Shape = typename Server::Shape;

            SerialRoutePartition();

            SerialRoutePartition(hpx::id_type component_id);

            SerialRoutePartition(hpx::future<hpx::id_type>&& component_id);

            SerialRoutePartition(hpx::shared_future<hpx::id_type>&& component_id);

            SerialRoutePartition(hpx::future<SerialRoutePartition>&& partition);

            SerialRoutePartition(hpx::id_type locality_id, Offset const& offset, Shape const& shape);

            SerialRoutePartition(
                hpx::id_type locality_id,
                Offset const& offset,
                Shape const& shape,
                RouteFragments&& route_fragments);

            SerialRoutePartition(SerialRoutePartition const&) = default;

            SerialRoutePartition(SerialRoutePartition&&) = default;

            ~SerialRoutePartition() = default;

            auto operator=(SerialRoutePartition const&) -> SerialRoutePartition& = default;

            auto operator=(SerialRoutePartition&&) -> SerialRoutePartition& = default;

            auto offset(hpx::launch::async_policy) const -> hpx::future<Offset>;

            auto offset(hpx::launch::sync_policy) const -> Offset;

            auto shape(hpx::launch::async_policy) const -> hpx::future<Shape>;

            auto shape(hpx::launch::sync_policy) const -> Shape;

            auto nr_routes(hpx::launch::async_policy) const -> hpx::future<Count>;

            auto nr_routes(hpx::launch::sync_policy) const -> Count;

            auto nr_route_fragments(hpx::launch::async_policy) const -> hpx::future<Count>;

            auto nr_route_fragments(hpx::launch::sync_policy) const -> Count;

            auto route_ids(hpx::launch::async_policy) const -> hpx::future<std::set<RouteID>>;

            auto route_ids(hpx::launch::sync_policy) const -> std::set<RouteID>;

            auto route_fragments(hpx::launch::async_policy, RouteID route_id) const
                -> hpx::future<std::vector<RouteFragment>>;

            auto route_fragments(hpx::launch::sync_policy, RouteID route_id) const
                -> std::vector<RouteFragment>;

            auto remote_route_fragment_locations(hpx::launch::async_policy) const
                -> hpx::future<std::vector<RouteFragmentLocation>>;

            auto remote_route_fragment_locations(hpx::launch::sync_policy) const
                -> std::vector<RouteFragmentLocation>;

            auto set_route_fragments(hpx::launch::async_policy, RouteFragments&& route_fragments)
                -> hpx::future<void>;

            auto set_route_fragments(hpx::launch::sync_policy, RouteFragments&& route_fragments) -> void;
    };


    namespace detail {

        template<typename RouteID, Rank r>
        class ArrayTraits<SerialRoutePartition<RouteID, r>>
        {

            public:

                constexpr static Rank rank = r;

                using Offset = typename SerialRoutePartition<RouteID, r>::Offset;

                using Shape = typename SerialRoutePartition<RouteID, r>::Shape;

                template<RouteID, Rank r_>
                using Partition = SerialRoutePartition<RouteID, r_>;
        };

    }  // namespace detail
}  // namespace lue
