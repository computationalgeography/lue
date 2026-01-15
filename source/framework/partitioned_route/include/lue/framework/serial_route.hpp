#pragma once
#include "lue/framework/core/array.hpp"
#include "lue/framework/partitioned_route/export.hpp"
#include "lue/framework/partitioned_route/serial_route_partition.hpp"


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
    class SerialRoute
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


            /*!
                @brief      Default-construct an instance
            */
            SerialRoute():

                _array_shape{},
                _starts{},
                _partitions{}

            {
                // Shape is filled with indeterminate values! This may or may not
                // matter, depending on what happens next with this instance. To be sure,
                // explicitly set the shape to empty.
                _array_shape.fill(0);

                assert_invariants();
            }


            /*!
                @brief      Construct an instance, given an array @a shape, a shared future to the
                            route @a starts, and @a partitions

                The array shape passed in must match the union of array partitions.
            */
            SerialRoute(Shape const& shape, hpx::shared_future<Starts>&& starts, Partitions&& partitions):

                _array_shape{shape},
                _starts{std::move(starts)},
                _partitions{std::move(partitions)}

            {
                assert_invariants();
            }


            /*!
                @brief      Construct an instance, given an array @a shape, a unique future to the
                            route @a starts, and @a partitions

                The array shape passed in must match the union of array partitions.
            */
            SerialRoute(Shape const& shape, hpx::future<Starts>&& starts, Partitions&& partitions):

                SerialRoute{shape, starts.share(), std::move(partitions)}

            {
                assert_invariants();
            }


            /*!
                @brief      Return the shape of the array
            */
            auto shape() const -> Shape const&
            {
                return _array_shape;
            }


            /*!
                @brief      Return a shared future to the collection of route starts
            */
            auto starts() const -> hpx::shared_future<Starts> const&
            {
                lue_hpx_assert(valid());

                return _starts;
            }


            /*!
                @brief      Return the collection of route partitions
            */
            auto partitions() const -> Partitions const&
            {
                return _partitions;
            }


            auto nr_routes() const -> Count
            {
                lue_hpx_assert(valid());
                lue_hpx_assert(is_ready());

                return static_cast<Count>(std::size(starts().get()));
            }


            auto valid() const -> bool
            {
                return _starts.valid();
            }


            auto is_ready() const -> bool
            {
                return _starts.is_ready();
            }


        private:

            void assert_invariants() const
            {
                // The array is either empty, or all partitions are valid (don't need to be ready)
                lue_hpx_assert(
                    (std::all_of(
                         _array_shape.begin(),
                         _array_shape.end(),
                         [](auto const extent) { return extent == 0; }) &&
                     nr_elements(_partitions.shape()) == 0) ||
                    (std::all_of(
                         _array_shape.begin(),
                         _array_shape.end(),
                         [](auto const extent) { return extent > 0; }) &&
                     std::all_of(
                         _partitions.begin(),
                         _partitions.end(),
                         [](auto const partition) { return partition.valid(); })));
            }


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
