#pragma once
#include "lue/framework/partitioned_array_decl.hpp"
#include "lue/hdf5/hyperslab.hpp"
#include <hpx/runtime_distributed/find_localities.hpp>
#include <map>

#if 0
#include <hpx/iostream.hpp>
#include <format>
#endif


namespace lue::detail {

#if 0
    template<class... Args>
    inline void print_debug(std::format_string<Args...> fmt, Args&&... args)
    {
        hpx::cout << std::format(
                         "DEBUG[{}]: {}\n",
                         hpx::agas::get_locality_id(),
                         std::format(fmt, std::forward<Args>(args)...))
                  << std::flush;
    }
#else
    template<class... Args>
    inline void print_debug([[maybe_unused]] std::format_string<Args...> fmt, [[maybe_unused]] Args&&... args)
    {
    }
#endif


    template<typename Shape>
    auto shape_to_hyperslab(Shape const& shape) -> hdf5::Hyperslab
    {
        // Return the hyperslab represented by the shape passed in
        return hdf5::Hyperslab{hdf5::Shape{shape.begin(), shape.end()}};
    }


    template<typename Shape>
    auto hyperslab_to_shape(hdf5::Hyperslab const& hyperslab) -> Shape
    {
        // Return the shape represented by the hyperslab passed in
        lue_hpx_assert(
            std::all_of(
                hyperslab.stride().begin(), hyperslab.stride().end(), [](auto const s) { return s == 1; }));
        auto const& count{hyperslab.count()};
        Shape shape;

        lue_hpx_assert(count.size() == shape.size());
        std::copy(count.begin(), count.end(), shape.begin());

        return shape;
    }


    template<Rank rank>
    auto hyperslab(Offset<Index, rank> const& partition_offset, Shape<Count, rank> const& partition_shape)
        -> hdf5::Hyperslab
    {
        hdf5::Offset offset(rank);
        std::copy(partition_offset.begin(), partition_offset.end(), offset.begin());

        hdf5::Count count(rank);
        std::copy(partition_shape.begin(), partition_shape.end(), count.begin());

        return hdf5::Hyperslab{offset, count};
    }


    template<Rank rank>
    auto hyperslab(
        hdf5::Offset const& array_hyperslab_start,
        Offset<Index, rank> const& partition_offset,
        Shape<Count, rank> const& partition_shape) -> hdf5::Hyperslab
    {
        hdf5::Offset offset(rank);
        lue_hpx_assert(array_hyperslab_start.size() == rank);
        std::transform(
            array_hyperslab_start.begin(),
            array_hyperslab_start.end(),
            partition_offset.begin(),
            offset.begin(),
            std::plus<hdf5::Offset::value_type>{});

        hdf5::Count count(rank);
        std::copy(partition_shape.begin(), partition_shape.end(), count.begin());

        return hdf5::Hyperslab{offset, count};
    }


    template<Rank rank>
    auto hyperslab(
        Offset<Index, rank> const& partition_offset,
        Shape<Count, rank> const& partition_shape,
        Index const location_in_time_idx,
        Index const time_step_idx) -> hdf5::Hyperslab
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


    template<Rank rank>
    auto hyperslab(
        hdf5::Offset const& array_hyperslab_start,
        Offset<Index, rank> const& partition_offset,
        Shape<Count, rank> const& partition_shape,
        Index const location_in_time_idx,
        Index const time_step_idx) -> hdf5::Hyperslab
    {
        hdf5::Offset offset(rank + 2);
        lue_hpx_assert(array_hyperslab_start.size() == rank);
        std::transform(
            array_hyperslab_start.begin(),
            array_hyperslab_start.end(),
            partition_offset.begin(),
            offset.begin() + 2,
            std::plus<hdf5::Offset::value_type>{});
        offset[0] = location_in_time_idx;
        offset[1] = time_step_idx;

        hdf5::Count count(rank + 2);
        std::copy(partition_shape.begin(), partition_shape.end(), count.begin() + 2);
        count[0] = 1;
        count[1] = 1;

        return hdf5::Hyperslab{offset, count};
    }


    template<typename PartitionServer>
    auto hyperslab(PartitionServer const& partition_server) -> hdf5::Hyperslab
    {
        // Return the part of the array represented by the partition server
        // instance passed in, as an HDF5 hyperslab. The result is useful
        // for performing I/O to the corresponding HDF5 dataset.
        return hyperslab(partition_server.offset(), partition_server.shape());
    }


    /*!
        @brief      .
        @param      .
        @return     array_hyperslab_start Offset of hyperslab represented
                    by the partitioned array of which @a partition_server
                    is a part.
        @exception  .
    */
    template<typename PartitionServer>
    auto hyperslab(hdf5::Offset const& array_hyperslab_start, PartitionServer const& partition_server)
        -> hdf5::Hyperslab
    {
        // Return the part of the array represented by the partition server
        // instance passed in, as an HDF5 hyperslab. The result is useful
        // for performing I/O to the corresponding HDF5 dataset.
        return hyperslab(array_hyperslab_start, partition_server.offset(), partition_server.shape());
    }


    template<typename PartitionServer>
    auto hyperslab(
        PartitionServer const& partition_server, Index const location_in_time_idx, Index const time_step_idx)
        -> hdf5::Hyperslab
    {
        // Return the part of the array represented by the partition server
        // instance passed in, as an HDF5 hyperslab. The result is useful
        // for performing I/O to the corresponding HDF5 dataset.
        return hyperslab(
            partition_server.offset(), partition_server.shape(), location_in_time_idx, time_step_idx);
    }


    template<typename PartitionServer>
    auto hyperslab(
        hdf5::Offset const& array_hyperslab_start,
        PartitionServer const& partition_server,
        Index const location_in_time_idx,
        Index const time_step_idx) -> hdf5::Hyperslab
    {
        // Return the part of the array represented by the partition server
        // instance passed in, as an HDF5 hyperslab. The result is useful
        // for performing I/O to the corresponding HDF5 dataset.
        return hyperslab(
            array_hyperslab_start,
            partition_server.offset(),
            partition_server.shape(),
            location_in_time_idx,
            time_step_idx);
    }


    /// template<typename Element, Rank rank>
    /// auto partitions_by_locality(PartitionedArray<Element, rank> const& array)
    ///     -> std::map<hpx::id_type, std::vector<PartitionT<PartitionedArray<Element, rank>>>>
    /// {
    ///     // The partitions in the array are distributed over localities. We
    ///     // perform I/O per locality, so for each of the localities, we must
    ///     // determine which partitions are stored there. Then we can attach
    ///     // a continuation to the group of partitions that will perform the I/O
    ///     // on the locality.

    ///     using Array = PartitionedArray<Element, rank>;
    ///     using Partitions = PartitionsT<Array>;
    ///     using Partition = PartitionT<Array>;

    ///     // Per locality the collection of partition component clients
    ///     std::map<hpx::id_type, std::vector<Partition>> result;

    ///     {
    ///         // All localities used by this array
    ///         Localities<rank> const& localities{array.localities()};
    ///         Partitions const& partitions{array.partitions()};
    ///         auto const nr_partitions{lue::nr_partitions(array)};

    ///         // Try to prevent reallocations while adding partitions to the
    ///         // collections. Assume the partitions are evenly distributed
    ///         // over all localities.
    ///         Count const max_nr_partitions_per_locality{
    ///             (nr_partitions / Count(hpx::find_all_localities().size())) + 1};

    ///         for (Index p = 0; p < nr_partitions; ++p)
    ///         {
    ///             hpx::id_type const locality{localities[p]};

    ///             if (result.find(locality) == result.end())
    ///             {
    ///                 // For this locality, we do not have created a
    ///                 // collection of partitions yet. Create one and
    ///                 // already reserve enough memory.
    ///                 std::vector<Partition> empty_partition_collection{};
    ///                 empty_partition_collection.reserve(max_nr_partitions_per_locality);
    ///                 result[locality] = std::move(empty_partition_collection);
    ///             }

    ///             lue_hpx_assert(result[locality].capacity() > result[locality].size());
    ///             result[locality].push_back(partitions[p]);
    ///         }
    ///     }

    ///     return result;
    /// }


    template<typename Element, Rank rank>
    auto partition_idxs_by_locality(PartitionedArray<Element, rank> const& array)
        -> std::map<hpx::id_type, std::vector<Index>>
    {
        // The partitions in the array are distributed over localities. We perform I/O per locality, so for
        // each of the localities, we must determine which partitions are stored there. Then we can attach a
        // continuation to the (group of) partitions that will perform the I/O on the locality.

        // Per locality the collection of linear indices of partition component clients
        std::map<hpx::id_type, std::vector<Index>> result;

        {
            // All localities used by this array
            Localities<rank> const& localities{array.localities()};
            auto const nr_partitions{lue::nr_partitions(array)};

            // Try to prevent reallocations while adding partitions to the collections. Assume the partitions
            // are evenly distributed over all localities.
            Count const max_nr_partitions_per_locality{
                (nr_partitions / Count(hpx::find_all_localities().size())) + 1};

            for (Index partition_idx = 0; partition_idx < nr_partitions; ++partition_idx)
            {
                hpx::id_type const locality{localities[partition_idx]};

                if (result.find(locality) == result.end())
                {
                    // For this locality, we do not have created a collection of partitions yet. Create one
                    // and already reserve (hopefully) enough memory.
                    std::vector<Index> empty_partition_collection{};
                    empty_partition_collection.reserve(max_nr_partitions_per_locality);
                    result[locality] = std::move(empty_partition_collection);
                }

                // This fails if the distribution of partitions over localities is not even. Update heuristics
                // if that is the case, if possible. Otherwise just remove the assert.
                lue_hpx_assert(result[locality].capacity() > result[locality].size());

                result[locality].push_back(partition_idx);
            }
        }

        lue_hpx_assert(result.size() <= hpx::find_all_localities().size());

        return result;
    }

}  // namespace lue::detail
