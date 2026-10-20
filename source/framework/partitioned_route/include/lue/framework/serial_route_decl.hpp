#pragma once
#include "lue/framework/core/array.hpp"
#include "lue/framework/partitioned_route/serial_route_partition_decl.hpp"


namespace lue {

    /*!
        @brief      Class for representing zero or more serial routes of array cells
        @tparam     RouteID Integral type for representing route IDs
        @tparam     rank Array rank

        A serial route is a route along a set of array cells. Any cell can be part of at most
        one route. The reason for cells to be part of a route and in which order is application
        defined. For example, in case of land-use allocation, routes can be defined along a
        decreasing suitability for crops (see decreasing_order()).

        The information about serial routes is stored in partitions
        (SerialRoutePartition). Partitions can be located in multiple processes.

        Per partition and per route, information about which cells are part the route in a
        collections of route fragments (SerialRouteFragment).
    */
    template<typename RouteID, Rank rank>
    class LUE_PARTITIONED_ROUTE_EXPORT SerialRoute
    {

            static_assert(std::is_integral_v<RouteID>);
            static_assert(std::is_integral_v<Rank>);

        public:

            using PartitionClient = SerialRoutePartition<RouteID, rank>;

            using Partitions = Array<PartitionClient, rank>;

            using Shape = typename Partitions::Shape;

            using FragmentLocation = typename SerialRouteFragment<rank>::Location;

            //! For each route the location of the first fragment
            using Starts = std::map<RouteID, FragmentLocation>;


            SerialRoute();

            SerialRoute(Shape const& shape, hpx::shared_future<Starts>&& starts, Partitions&& partitions);

            SerialRoute(Shape const& shape, hpx::future<Starts>&& starts, Partitions&& partitions);

            auto shape() const -> Shape const&;

            auto starts() const -> hpx::shared_future<Starts> const&;

            auto partitions() const -> Partitions const&;

            auto nr_routes() const -> Count;

            auto valid() const -> bool;

            auto is_ready() const -> bool;

        private:

            void assert_invariants() const;

            //! Shape of the array whose cells are (potentially) part of routes
            Shape _array_shape;

            //! Future to the collection of starts of the routes
            hpx::shared_future<Starts> _starts;

            //! Partitions containing the route fragments for the routes
            Partitions _partitions;
    };


    namespace detail {

        template<typename RouteID, Rank r>
        class ArrayTraits<SerialRoute<RouteID, r>>
        {

            public:

                static constexpr Rank rank = r;

                static constexpr bool is_partitioned_array{true};

                using Shape = typename SerialRoute<RouteID, r>::Shape;

                template<Rank r_>
                using Partition = typename SerialRoute<RouteID, r_>::PartitionClient;

                template<Rank r_>
                using Partitions = typename SerialRoute<RouteID, r_>::Partitions;

                using Offset = typename Partition<r>::Offset;
        };


        template<typename RouteID, Rank r>
        class ArrayTraits<Array<SerialRoutePartition<RouteID, r>, r>>
        {

            public:

                constexpr static Rank rank = r;

                using Offset = typename SerialRoutePartition<RouteID, r>::Offset;

                using Shape = typename SerialRoutePartition<RouteID, r>::Shape;

                template<typename RouteID_, Rank r_>
                using Partition = SerialRoutePartition<RouteID_, r_>;

                template<typename RouteID_, Rank r_>
                using Partitions = typename SerialRoute<RouteID_, r_>::Partitions;
        };

    }  // namespace detail
}  // namespace lue
