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

            using FragmentLocation = typename SerialRouteFragment<rank>::Location;

            //! For each route the location of the first fragment
            using Starts = std::map<RouteID, FragmentLocation>;

            using StartsFuture = hpx::shared_future<Starts>;

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

            SerialRoute(Shape const& shape, StartsFuture&& starts, Partitions&& partitions):

                _array_shape{shape},
                _starts{std::move(starts)},
                _partitions{std::move(partitions)}

            {
                assert_invariants();
            }


            SerialRoute(Shape const& shape, hpx::future<Starts>&& starts, Partitions&& partitions):

                SerialRoute{shape, starts.share(), std::move(partitions)}

            {
                assert_invariants();
            }


            Shape const& shape() const
            {
                return _array_shape;
            }


            StartsFuture starts() const
            {
                return _starts;
            }


            Partitions const& partitions() const
            {
                return _partitions;
            }


            hpx::future<Count> nr_routes() const
            {
                lue_hpx_assert(is_valid());

                return _starts.then([](auto const& starts)
                                    { return static_cast<Count>(std::size(starts.get())); });
            }


            bool is_valid() const
            {
                return _starts.valid();
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


            Shape _array_shape;

            StartsFuture _starts;

            Partitions _partitions;
    };


    namespace detail {

        template<Rank r>
        class ArrayTraits<SerialRoute<r>>
        {

            public:

                static constexpr Rank rank = r;

                static constexpr bool is_partitioned_array{true};

                using Shape = typename SerialRoute<r>::Shape;

                template<Rank r_>
                using Partition = typename SerialRoute<r_>::PartitionClient;

                template<Rank r_>
                using Partitions = typename SerialRoute<r_>::Partitions;

                using Offset = typename Partition<r>::Offset;
        };


        template<Rank r>
        class ArrayTraits<Array<typename SerialRoute<r>::PartitionClient, r>>
        {

            public:

                constexpr static Rank rank = r;

                using Offset = typename SerialRoute<r>::PartitionClient::Offset;

                using Shape = typename SerialRoute<r>::Shape;

                template<Rank r_>
                using Partition = typename SerialRoute<r_>::PartitionClient;

                template<Rank r_>
                using Partitions = typename SerialRoute<r_>::Partitions;
        };

    }  // namespace detail
}  // namespace lue
