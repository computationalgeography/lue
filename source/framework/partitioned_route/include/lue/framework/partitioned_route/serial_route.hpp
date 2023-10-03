#pragma once
#include "lue/framework/partitioned_route/serial_route_partition.hpp"


namespace lue {

    template<Rank rank>
    class SerialRoute
    {

        private:

            using PartitionClient = SerialRoutePartition<rank>;

            using PartitionServer = typename PartitionClient::Server;

        public:

            using RouteID = typename PartitionServer::RouteID;

            // using RouteFragment = typename PartitionServer::RouteFragment;


            Count nr_routes() const
            {
                return static_cast<Count>(std::size(_start_of_routes));
            }

        private:

            //! For each route the location of the first fragment
            using StartOfRoutes = std::map<RouteID, typename SerialRouteFragment<rank>::Location>;

            //! Collection of partitions in which routes start
            StartOfRoutes _start_of_routes;
    };

}  // namespace lue
