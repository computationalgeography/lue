#pragma once
#include "lue/framework/core/array.hpp"
#include "lue/framework/partitioned_route/serial_route_partition.hpp"
#include <algorithm>


namespace lue {

    template<Rank rank>
    class SerialRoute
    {

        public:

            using PartitionClient = SerialRoutePartition<rank>;

            using PartitionServer = typename PartitionClient::Server;

            using Partitions = Array<PartitionClient, rank>;

            using Shape = typename Partitions::Shape;

            using RouteID = typename PartitionServer::RouteID;

            // using RouteFragment = typename PartitionServer::RouteFragment;

            SerialRoute():

                _array_shape{},
                _partitions{}

            {
                // Shape is filled with indeterminate values! This may or may not
                // matter, depending on what happens next with this instance. To be sure,
                // explicitly set the shape to empty.
                _array_shape.fill(0);

                assert_invariants();
            }

            SerialRoute(Shape const& array_shape, Partitions&& partitions):

                _array_shape{array_shape},
                _partitions(std::move(partitions))

            {
                assert_invariants();
            }


            Shape const& array_shape() const
            {
                return _array_shape;
            }


            Partitions const& partitions() const
            {
                return _partitions;
            }


            Count nr_routes() const
            {
                return static_cast<Count>(std::size(_start_of_routes));
            }

        private:

            void assert_invariants() const
            {
                // // The array is either empty, or all partitions are valid (don't need to be ready)
                // lue_hpx_assert(
                //     (std::all_of(
                //          _array_shape.begin(), _array_shape.end(), [](auto const extent) { return extent ==
                //          0; }) &&
                //      nr_elements(_partitions.shape()) == 0) ||
                //     (std::all_of(
                //          _array_shape.begin(), _array_shape.end(), [](auto const extent) { return extent >
                //          0; }) &&
                //      std::all_of(
                //          _partitions.begin(),
                //          _partitions.end(),
                //          [](auto const partition) { return partition.valid(); })));
            }


            //! For each route the location of the first fragment
            using StartOfRoutes = std::map<RouteID, typename SerialRouteFragment<rank>::Location>;

            Shape _array_shape;

            Partitions _partitions;

            //! Collection of partitions in which routes start
            StartOfRoutes _start_of_routes;
    };

}  // namespace lue
