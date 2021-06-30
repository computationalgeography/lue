#pragma once
#include "lue/framework/core/define.hpp"
#include "lue/framework/core/assert.hpp"
#include "lue/framework/core/indices.hpp"
#include "lue/framework/core/offset.hpp"
#include "lue/framework/core/shape.hpp"
#include <hpx/include/lcos.hpp>
#include <cstdint>


namespace lue::detail {

    template<
        Rank rank,
        Index dimension=rank>
    constexpr Count nr_neighbours()
    {
        // Given the rank of a partition, how many neighboring partitions
        // are there?

        // 1D: 1 * 2 * (1 - 0) → 2
        // 2D: 1 * 2 * (2 - 0) + 2 * 2 * (2 - 1) → 4 + 4 → 8
        // 3D: 1 * 2 * (3 - 0) + 2 * 2 * (3 - 1) + 3 * 2 * (3 - 2) → 6 + 8 + 12 → 26
        //
        // Given rank r, for each current dimension d є (0, r]:
        //     d * 2 * (r - (d - 1))

        if constexpr(dimension > 0) {
            return dimension * 2 * (rank - (dimension - 1)) + nr_neighbours<rank, dimension - 1>();
        }
        else {
            return 0;
        }
    }


    // TODO Without the namespace, the names clash with flow direction
    // templates. Refactor.
    namespace accu {

        enum /* class */ Direction
        {
            north = 0,
            north_east = 1,
            east = 2,
            south_east = 3,
            south = 4,
            south_west = 5,
            west = 6,
            north_west = 7,
        };

    }


    template<
        typename T,
        Rank rank>
    class Communicator
    {

        // For now, assume we are working with a 2D world of
        // neighbours to communicate with
        static_assert(rank == 2);
        static_assert(nr_neighbours<rank>() == 8);


        public:

            using Value = T;
            using Channel = hpx::lcos::channel<T>;
            using Channels = std::array<Channel, nr_neighbours<rank>()>;
            using Shape = lue::Shape<Count, rank>;
            using Indices = lue::Indices<Index, rank>;
            using Offset = lue::Offset<Index, rank>;


            Communicator()=default;


            Communicator(
                hpx::naming::id_type const locality_id,
                std::string const& name,
                Shape const& shape,
                Indices const& idxs):

                _shape{shape},
                _idxs{idxs},
                _idx{linear_idx(shape, idxs)},
                _send_channels{},
                _receive_channels{}

            {
                [[maybe_unused]] auto nr_valid_channels =
                    [](Channels const& channels)
                    {
                        return std::count_if(channels.begin(), channels.end(),
                            [](Channel const& channel) { return bool(channel); });
                    };

                [[maybe_unused]] auto same_channels_are_valid =
                    [](Channels const& channels1, Channels const& channels2)
                    {
                        return std::equal(channels1.begin(), channels1.end(), channels2.begin(),
                            [](Channel const& channel1, Channel const& channel2)
                            {
                                return
                                    (bool(channel1) && bool(channel2)) ||
                                    (!bool(channel1) && !bool(channel2));
                            });
                    };

                lue_hpx_assert(nr_valid_channels(_send_channels) == 0);
                lue_hpx_assert(nr_valid_channels(_receive_channels) == 0);


                auto const [extent0, extent1] = shape;
                auto const [idx0, idx1] = idxs;

                lue_hpx_assert(extent0 > 0 && extent1 > 0);

                if(extent0 > 1 || extent1 > 1)
                {
                    // There is at least one neighbour. Setup channels
                    // to our neigbouring communicators.

                    if(idx0 > 0)
                    {
                        setup_channels(locality_id, name, accu::Direction::north);

                        if(idx1 > 0)
                        {
                            setup_channels(locality_id, name, accu::Direction::north_west);
                        }

                        if(idx1 < extent1 - 1)
                        {
                            setup_channels(locality_id, name, accu::Direction::north_east);
                        }
                    }

                    if(idx0 < extent0 - 1)
                    {
                        setup_channels(locality_id, name, accu::Direction::south);

                        if(idx1 > 0)
                        {
                            setup_channels(locality_id, name, accu::Direction::south_west);
                        }

                        if(idx1 < extent1 - 1)
                        {
                            setup_channels(locality_id, name, accu::Direction::south_east);
                        }
                    }

                    if(idx1 > 0)
                    {
                        setup_channels(locality_id, name, accu::Direction::west);
                    }

                    if(idx1 < extent1 - 1)
                    {
                        setup_channels(locality_id, name, accu::Direction::east);
                    }
                }

                lue_hpx_assert(nr_valid_channels(_send_channels) == nr_valid_channels(_receive_channels));
                lue_hpx_assert(same_channels_are_valid(_send_channels, _receive_channels));
            }


            bool has_neighbour(
                accu::Direction const& direction) const
            {
                return bool(send_channel(direction));
            }


            void send(
                accu::Direction const& direction,
                T const& value)
            {
                lue_hpx_assert(has_neighbour(direction));

                auto& channel{send_channel(direction)};

                channel.set(value);
            }


            void send(
                T const& value)
            {
                for(auto& send_channel: _send_channels)
                {
                    if(send_channel)
                    {
                        send_channel.set(value);
                    }
                }
            }


            void done_sending()
            {
                for(auto& send_channel: _send_channels)
                {
                    if(send_channel)
                    {
                        send_channel.close();
                    }
                }
            }


            void done_sending(
                T const& sentinel_value)
            {
                for(auto& send_channel: _send_channels)
                {
                    if(send_channel)
                    {
                        send_channel.set(sentinel_value);
                        send_channel.close();
                    }
                }
            }


            Channels& receive_channels()
            {
                return _receive_channels;
            }


            Channel& receive_channel(
                accu::Direction const direction)
            {
                return _receive_channels[direction];
            }


        protected:

            friend class hpx::serialization::access;


            template<typename Archive>
            void serialize(
                Archive& archive,
                [[maybe_unused]] unsigned int const version)
            {
                archive & _shape & _idxs & _idx & _send_channels & _receive_channels;
            }


        private:

            static std::size_t linear_idx(
                Shape const& shape,
                Indices const& idxs)
            {
                auto const [extent0, extent1] = shape;
                auto const [idx0, idx1] = idxs;

                return (idx0 * extent1) + idx1;
            }


            static std::string channel_name(
                std::string const& communicator_name,
                accu::Direction const direction)
            {
                return communicator_name + "/" + std::to_string(direction) + "/";
            }


            Channel const & send_channel(
                accu::Direction const direction) const
            {
                return _send_channels[direction];
            }


            Channel& send_channel(
                accu::Direction const direction)
            {
                return _send_channels[direction];
            }


            std::size_t neighbour_idx(
                accu::Direction const direction) const
            {
                static std::array<Offset, nr_neighbours<rank>()> const offset{
                        Offset{-1,  0},
                        Offset{-1,  1},
                        Offset{ 0,  1},
                        Offset{ 1,  1},
                        Offset{ 1,  0},
                        Offset{ 1, -1},
                        Offset{ 0, -1},
                        Offset{-1, -1},
                    };

                auto const [idx0, idx1] = _idxs;
                auto const [offset0, offset1] = offset[direction];

                return linear_idx(_shape, Indices{idx0 + offset0, idx1 + offset1});
            }


            void setup_send_channel(
                hpx::naming::id_type const locality_id,
                std::string const& communicator_name,
                accu::Direction const direction)
            {
                lue_hpx_assert(!send_channel(direction));

                send_channel(direction) = Channel{locality_id};
                lue_hpx_assert(send_channel(direction));

                [[maybe_unused]] hpx::future<bool> result{
                    hpx::register_with_basename(
                        channel_name(communicator_name, direction), send_channel(direction), _idx)};
                // lue_hpx_assert(result.get());
#ifndef NDEBUG
                result.then(
                    [](hpx::future<bool>&& result)
                    {
                        lue_hpx_assert(result.get());
                    });
#endif
            }


            void setup_receive_channel(
                std::string const& communicator_name,
                accu::Direction const direction)
            {
                lue_hpx_assert(!receive_channel(direction));

                // Directions to us, from communicator at direction passed in
                static std::array<accu::Direction, nr_neighbours<rank>()> sender_direction{
                        accu::Direction::south,
                        accu::Direction::south_west,
                        accu::Direction::west,
                        accu::Direction::north_west,
                        accu::Direction::north,
                        accu::Direction::north_east,
                        accu::Direction::east,
                        accu::Direction::south_east,
                    };

                receive_channel(direction) =
                    hpx::find_from_basename<Channel>(
                        channel_name(communicator_name, sender_direction[direction]), neighbour_idx(direction));
                lue_hpx_assert(receive_channel(direction));
            }


            void setup_channels(
                hpx::naming::id_type const locality_id,
                std::string const& communicator_name,
                accu::Direction const direction)
            {
                setup_send_channel(locality_id, communicator_name, direction);
                setup_receive_channel(communicator_name, direction);
            }


            Shape _shape;

            Indices _idxs;

            std::size_t _idx;

            Channels _send_channels;

            Channels _receive_channels;

    };

}  // namespace lue::detail
