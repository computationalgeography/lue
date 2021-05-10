#pragma once
#include "lue/framework/partitioned_array.hpp"
#include "lue/hdf5/hyperslab.hpp"
#include <map>


namespace lue::detail {

    hpx::future<void> when_all_get(std::vector<hpx::future<void>>&& futures);

    std::array<std::string, 4> parse_array_pathname(std::string const& array_pathname);


    template<
        typename Shape>
    Shape hyperslab_to_shape(
        hdf5::Hyperslab const& hyperslab)
    {
        // Return the shape represented by the hyperslab passed in
        lue_hpx_assert(std::all_of(hyperslab.stride().begin(), hyperslab.stride().end(),
            [](auto const s) { return s == 1; }));
        auto const& count{hyperslab.count()};
        Shape shape;

        lue_hpx_assert(count.size() == shape.size());
        std::copy(count.begin(), count.end(), shape.begin());

        return shape;
    }


    template<
        Rank rank>
    hdf5::Hyperslab hyperslab(
        Offset<Index, rank> const& partition_offset,
        Shape<Count, rank> const& partition_shape)
    {
        hdf5::Offset offset(rank);
        std::copy(partition_offset.begin(), partition_offset.end(), offset.begin());

        hdf5::Count count(rank);
        std::copy(partition_shape.begin(), partition_shape.end(), count.begin());

        return hdf5::Hyperslab{offset, count};
    }


    template<
        Rank rank>
    hdf5::Hyperslab hyperslab(
        Offset<Index, rank> const& partition_offset,
        Shape<Count, rank> const& partition_shape,
        Index const location_in_time_idx,
        Index const time_step_idx)
    {
        hdf5::Offset offset(rank + 2);
        std::copy(partition_offset.begin(), partition_offset.end(), offset.begin() + 2);
        offset[0] = location_in_time_idx;
        offset[1] = time_step_idx;

        hdf5::Count count(rank + 2);
        std::copy(partition_shape.begin(), partition_shape.end(), count.begin() + 2);
        count[0] = 1;
        count[1] = 1;

        return hdf5::Hyperslab{offset, count};
    }


    template<
        typename PartitionServer>
    hdf5::Hyperslab hyperslab(
        PartitionServer const& partition_server)
    {
        // Return the part of the array represented by the partition server
        // instance passed in, as an HDF5 hyperslab. The result is useful
        // for performing I/O to the corresponding HDF5 dataset.
        return hyperslab(partition_server.offset(), partition_server.shape());
    }


    template<
        typename PartitionServer>
    hdf5::Hyperslab hyperslab(
        PartitionServer const& partition_server,
        Index const location_in_time_idx,
        Index const time_step_idx)
    {
        // Return the part of the array represented by the partition server
        // instance passed in, as an HDF5 hyperslab. The result is useful
        // for performing I/O to the corresponding HDF5 dataset.
        return hyperslab(
            partition_server.offset(), partition_server.shape(), location_in_time_idx, time_step_idx);
    }


    template<
        typename Element,
        Rank rank>
    std::map<hpx::id_type, std::vector<PartitionT<PartitionedArray<Element, rank>>>> partitions_by_locality(
        PartitionedArray<Element, rank> const& array)
    {
        // The partitions in the array are distributed over localities. We
        // perform I/O per locality, so for each of the localities, we must
        // determine which partitions are stored there. Then we can attach
        // a continuation to the group of partitions that will perform the read
        // on the locality.

        using Array = PartitionedArray<Element, rank>;
        using Partitions = PartitionsT<Array>;
        using Partition = PartitionT<Array>;

        std::map<hpx::id_type, std::vector<Partition>> result;

        {
            Localities<rank> const& localities{array.localities()};
            Partitions const& partitions{array.partitions()};
            auto const nr_partitions{lue::nr_partitions(array)};

            // Try to prevent reallocations while adding partitions to the
            // collections. Assume the partitions are evenly distributed
            // over all localities.
            Count const max_nr_partitions_per_locality{
                (nr_partitions / Count(hpx::find_all_localities().size())) + 1};

            for(Index p = 0; p < nr_partitions; ++p)
            {
                if(result.find(localities[p]) == result.end())
                {
                    std::vector<Partition> empty_partition_collection{};
                    empty_partition_collection.reserve(max_nr_partitions_per_locality);
                    result[localities[p]] = std::move(empty_partition_collection);
                }

                lue_hpx_assert(result[localities[p]].capacity() > result[localities[p]].size());
                result[localities[p]].push_back(partitions[p]);
            }
        }

        return result;
    }

}  // namespace lue::detail
