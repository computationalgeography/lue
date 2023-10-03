#pragma once
#include "lue/framework/partitioned_route/export.hpp"
#include "lue/framework/partitioned_route/server/serial_route_partition.hpp"


namespace lue {

    /*!
        @brief      Class for serial route partition component client instances

        Each instance is associated with a single serial route partition component
        server instance. Multiple instances can be associated with a single server instance.
    */
    template<Rank rank>
    class LUE_FPR_EXPORT SerialRoutePartition:
        public hpx::components::client_base<SerialRoutePartition<rank>, server::SerialRoutePartition<rank>>

    {

        public:

            using Server = server::SerialRoutePartition<rank>;

            using Base = hpx::components::client_base<SerialRoutePartition<rank>, Server>;

            using RouteID = typename Server::RouteID;

            SerialRoutePartition();

            SerialRoutePartition(hpx::id_type component_id);

            SerialRoutePartition(hpx::future<hpx::id_type>&& component_id);

            SerialRoutePartition(SerialRoutePartition const&) = default;

            SerialRoutePartition(SerialRoutePartition&&) = default;

            ~SerialRoutePartition() = default;

            SerialRoutePartition& operator=(SerialRoutePartition const&) = default;

            SerialRoutePartition& operator=(SerialRoutePartition&&) = default;

            hpx::future<Count> nr_routes() const;

            hpx::future<Count> nr_route_fragments() const;

            hpx::future<std::vector<RouteID>> route_ids() const;
    };

}  // namespace lue
