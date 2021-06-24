#pragma once
#include "lue/framework/algorithm/detail/communicator.hpp"
#include <array>
#include <tuple>
#include <vector>


namespace lue::detail {

    inline std::tuple<accu::Direction, std::array<Index, 2>> destination_cell(
        Count const extent0,
        Count const extent1,
        Index const idx0,
        Index const idx1,
        Index const offset0,
        Index const offset1)
    {
        lue_hpx_assert(
            (idx0 == 0 || idx0 == extent0 - 1) ||
            (idx1 == 0 || idx1 == extent1 - 1));
        lue_hpx_assert(offset0 >= -1 && offset0 <= 1);
        lue_hpx_assert(offset1 >= -1 && offset1 <= 1);

        // Which neighbouring partition to send a message to
        accu::Direction direction;

        // Idxs of cell in the neighbouring partition
        std::array<Index, 2> idxs;

        if(idx0 == 0)
        {
            // north border
            if(idx1 == 0)
            {
                // north-west source cell → determine destination
                if(offset0 >= 0)
                {
                    lue_hpx_assert(offset1 == -1);
                    direction = accu::Direction::west;
                    idxs = {idx0 + offset0, extent1 - 1};
                }
                else if(offset1 >= 0)
                {
                    lue_hpx_assert(offset0 == -1);
                    direction = accu::Direction::north;
                    idxs = {extent0 - 1, idx1 + offset1};
                }
                else
                {
                    lue_hpx_assert(offset0 == -1);
                    lue_hpx_assert(offset1 == -1);
                    direction = accu::Direction::north_west;
                    idxs = {extent0 - 1, extent1 - 1};
                }
            }
            else if(idx1 == extent1 - 1)
            {
                // north-east source cell → determine destination
                if(offset0 >= 0)
                {
                    lue_hpx_assert(offset1 == 1);
                    direction = accu::Direction::east;
                    idxs = {idx0 + offset0, 0};
                }
                else if(offset1 <= 0)
                {
                    lue_hpx_assert(offset0 == -1);
                    direction = accu::Direction::north;
                    idxs = {extent0 - 1, idx1 + offset1};
                }
                else
                {
                    lue_hpx_assert(offset0 == -1);
                    lue_hpx_assert(offset1 == 1);
                    direction = accu::Direction::north_east;
                    idxs = {extent0 - 1, 0};
                }
            }
            else
            {
                // north source cell → determine destination
                lue_hpx_assert(offset0 == -1);
                direction = accu::Direction::north;
                idxs = {extent0 - 1, idx1 + offset1};
            }
        }
        else if(idx0 == extent0 - 1)
        {
            // south border
            if(idx1 == 0)
            {
                // south-west source cell → determine destination
                if(offset0 <= 0)
                {
                    lue_hpx_assert(offset1 == -1);
                    direction = accu::Direction::west;
                    idxs = {idx0 + offset0, extent1 - 1};
                }
                else if(offset1 >= 0)
                {
                    lue_hpx_assert(offset0 == 1);
                    direction = accu::Direction::south;
                    idxs = {0, idx1 + offset1};
                }
                else
                {
                    lue_hpx_assert(offset0 == 1);
                    lue_hpx_assert(offset1 == -1);
                    direction = accu::Direction::south_west;
                    idxs = {0, extent1 - 1};
                }
            }
            else if(idx1 == extent1 - 1)
            {
                // south-east source cell → determine destination
                if(offset0 <= 0)
                {
                    lue_hpx_assert(offset1 == 1);
                    direction = accu::Direction::east;
                    idxs = {idx0 + offset0, 0};
                }
                else if(offset1 <= 0)
                {
                    lue_hpx_assert(offset0 == 1);
                    direction = accu::Direction::south;
                    idxs = {0, idx1 + offset1};
                }
                else
                {
                    lue_hpx_assert(offset0 == 1);
                    lue_hpx_assert(offset1 == 1);
                    direction = accu::Direction::south_east;
                    idxs = {0, 0};
                }
            }
            else
            {
                // south source cell → determine destination
                lue_hpx_assert(offset0 == 1);
                direction = accu::Direction::south;
                idxs = {0, idx1 + offset1};
            }
        }
        else if(idx1 == 0)
        {
            // west source cell → determine destination
            lue_hpx_assert(offset1 == -1);
            direction = accu::Direction::west;
            idxs = {idx0 + offset0, extent1 - 1};

        }
        else if(idx1 == extent1 - 1)
        {
            // east source cell → determine destination
            lue_hpx_assert(offset1 == 1);
            direction = accu::Direction::east;
            idxs = {idx0 + offset0, 0};
        }

        return std::make_tuple(direction, idxs);
    }


    template<
        Rank rank>
    class InflowCountCommunicator:
        public Communicator<std::vector<Index>, rank>
    {

        public:

            using Base = Communicator<std::vector<Index>, rank>;


            InflowCountCommunicator()=default;


            InflowCountCommunicator(
                hpx::naming::id_type const locality_id,
                std::string const& scope,
                Shape<Count, rank> const& shape_in_partitions,
                Indices<Index, rank> const& partition_idxs):

                Base{locality_id, "/lue/" + scope, shape_in_partitions, partition_idxs}

            {
            }


        private:

            friend class hpx::serialization::access;


            template<typename Archive>
            void serialize(
                Archive& archive,
                unsigned int const version)
            {
                Base::serialize(archive, version);
            }

    };

}  // namespace lue::detail
