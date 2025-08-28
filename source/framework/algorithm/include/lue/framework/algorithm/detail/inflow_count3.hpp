#pragma once
#include "lue/framework/algorithm/detail/communicator.hpp"
#include <tuple>
#include <vector>


namespace lue::detail {

    inline auto destination_cell(
        Count const extent0,
        Count const extent1,
        Index const idx0,
        Index const idx1,
        Index const offset0,
        Index const offset1) -> std::tuple<accu::Direction, Index>
    {
        lue_hpx_assert((idx0 == 0 || idx0 == extent0 - 1) || (idx1 == 0 || idx1 == extent1 - 1));
        lue_hpx_assert(offset0 >= -1 && offset0 <= 1);
        lue_hpx_assert(offset1 >= -1 && offset1 <= 1);

        // Which neighbouring partition to send a message to
        accu::Direction direction{};

        // Idx of cell in the neighbouring partition
        Index idx{-1};

        if (idx0 == 0)
        {
            // north border
            if (idx1 == 0)
            {
                // north-west source cell → determine destination
                if (offset0 >= 0)
                {
                    lue_hpx_assert(offset1 == -1);
                    direction = accu::Direction::west;
                    idx = idx0 + offset0;
                }
                else if (offset1 >= 0)
                {
                    lue_hpx_assert(offset0 == -1);
                    direction = accu::Direction::north;
                    idx = idx1 + offset1;
                }
                else
                {
                    lue_hpx_assert(offset0 == -1);
                    lue_hpx_assert(offset1 == -1);
                    direction = accu::Direction::north_west;
                    idx = -1;  // Whatever
                }
            }
            else if (idx1 == extent1 - 1)
            {
                // north-east source cell → determine destination
                if (offset0 >= 0)
                {
                    lue_hpx_assert(offset1 == 1);
                    direction = accu::Direction::east;
                    idx = idx0 + offset0;
                }
                else if (offset1 <= 0)
                {
                    lue_hpx_assert(offset0 == -1);
                    direction = accu::Direction::north;
                    idx = idx1 + offset1;
                }
                else
                {
                    lue_hpx_assert(offset0 == -1);
                    lue_hpx_assert(offset1 == 1);
                    direction = accu::Direction::north_east;
                    idx = 0;  // Whatever
                }
            }
            else
            {
                // north source cell → determine destination
                lue_hpx_assert(offset0 == -1);
                direction = accu::Direction::north;
                idx = idx1 + offset1;
            }
        }
        else if (idx0 == extent0 - 1)
        {
            // south border
            if (idx1 == 0)
            {
                // south-west source cell → determine destination
                if (offset0 <= 0)
                {
                    lue_hpx_assert(offset1 == -1);
                    direction = accu::Direction::west;
                    idx = idx0 + offset0;
                }
                else if (offset1 >= 0)
                {
                    lue_hpx_assert(offset0 == 1);
                    direction = accu::Direction::south;
                    idx = idx1 + offset1;
                }
                else
                {
                    lue_hpx_assert(offset0 == 1);
                    lue_hpx_assert(offset1 == -1);
                    direction = accu::Direction::south_west;
                    idx = -1;  // Whatever
                }
            }
            else if (idx1 == extent1 - 1)
            {
                // south-east source cell → determine destination
                if (offset0 <= 0)
                {
                    lue_hpx_assert(offset1 == 1);
                    direction = accu::Direction::east;
                    idx = idx0 + offset0;
                }
                else if (offset1 <= 0)
                {
                    lue_hpx_assert(offset0 == 1);
                    direction = accu::Direction::south;
                    idx = idx1 + offset1;
                }
                else
                {
                    lue_hpx_assert(offset0 == 1);
                    lue_hpx_assert(offset1 == 1);
                    direction = accu::Direction::south_east;
                    idx = -1;  // Whatever
                }
            }
            else
            {
                // south source cell → determine destination
                lue_hpx_assert(offset0 == 1);
                direction = accu::Direction::south;
                idx = idx1 + offset1;
            }
        }
        else if (idx1 == 0)
        {
            // west source cell → determine destination
            lue_hpx_assert(offset1 == -1);
            direction = accu::Direction::west;
            idx = idx0 + offset0;
        }
        else if (idx1 == extent1 - 1)
        {
            // east source cell → determine destination
            lue_hpx_assert(offset1 == 1);
            direction = accu::Direction::east;
            idx = idx0 + offset0;
        }

        return std::make_tuple(direction, idx);
    }


    template<Rank rank>
    class InflowCountCommunicator: public Communicator<std::vector<Index>, rank>
    {

        public:

            using Base = Communicator<std::vector<Index>, rank>;


            InflowCountCommunicator() = default;


            InflowCountCommunicator(
                hpx::id_type const locality_id,
                std::string const& basename,
                lue::Shape<Count, rank> const& shape_in_partitions,
                lue::Indices<Index, rank> const& partition_idxs):

                Base{locality_id, basename, shape_in_partitions, partition_idxs}

            {
            }


        private:

            friend class hpx::serialization::access;


            template<typename Archive>
            void serialize(Archive& archive, unsigned int const version)
            {
                Base::serialize(archive, version);
            }
    };

}  // namespace lue::detail
