#pragma once
#include "lue/framework/partitioned_array.hpp"
#include "lue/framework/core/component.hpp"
#include "lue/hdf5/hyperslab.hpp"
#include <map>


namespace lue::detail {

    hpx::future<void> when_all_get(std::vector<hpx::future<void>>&& futures);


    template<
        typename Shape>
    hdf5::Hyperslab shape_to_hyperslab(
        Shape const& shape)
    {
        // Return the hyperslab represented by the shape passed in
        return hdf5::Hyperslab{hdf5::Shape{shape.begin(), shape.end()}};
    }


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
        hdf5::Offset const& array_hyperslab_start,
        Offset<Index, rank> const& partition_offset,
        Shape<Count, rank> const& partition_shape)
    {
        hdf5::Offset offset(rank);
        lue_hpx_assert(array_hyperslab_start.size() == rank);
        std::transform(
            array_hyperslab_start.begin(), array_hyperslab_start.end(),
            partition_offset.begin(),
            offset.begin(), std::plus<hdf5::Offset::value_type>{});

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
        Rank rank>
    hdf5::Hyperslab hyperslab(
        hdf5::Offset const& array_hyperslab_start,
        Offset<Index, rank> const& partition_offset,
        Shape<Count, rank> const& partition_shape,
        Index const location_in_time_idx,
        Index const time_step_idx)
    {
        hdf5::Offset offset(rank + 2);
        lue_hpx_assert(array_hyperslab_start.size() == rank);
        std::transform(
            array_hyperslab_start.begin(), array_hyperslab_start.end(),
            partition_offset.begin(),
            offset.begin() + 2, std::plus<hdf5::Offset::value_type>{});
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


    /*!
        @brief      .
        @param      .
        @return     array_hyperslab_start Offset of hyperslab represented
                    by the partitioned array of which @a partition_server
                    is a part.
        @exception  .
    */
    template<
        typename PartitionServer>
    hdf5::Hyperslab hyperslab(
        hdf5::Offset const& array_hyperslab_start,
        PartitionServer const& partition_server)
    {
        // Return the part of the array represented by the partition server
        // instance passed in, as an HDF5 hyperslab. The result is useful
        // for performing I/O to the corresponding HDF5 dataset.
        return hyperslab(array_hyperslab_start, partition_server.offset(), partition_server.shape());
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
            partition_server.offset(), partition_server.shape(),
            location_in_time_idx, time_step_idx);
    }


    template<
        typename PartitionServer>
    hdf5::Hyperslab hyperslab(
        hdf5::Offset const& array_hyperslab_start,
        PartitionServer const& partition_server,
        Index const location_in_time_idx,
        Index const time_step_idx)
    {
        // Return the part of the array represented by the partition server
        // instance passed in, as an HDF5 hyperslab. The result is useful
        // for performing I/O to the corresponding HDF5 dataset.
        return hyperslab(
            array_hyperslab_start, partition_server.offset(), partition_server.shape(),
            location_in_time_idx, time_step_idx);
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
        // a continuation to the group of partitions that will perform the I/O
        // on the locality.

        using Array = PartitionedArray<Element, rank>;
        using Partitions = PartitionsT<Array>;
        using Partition = PartitionT<Array>;

        // Per locality the collection of partition component clients
        std::map<hpx::id_type, std::vector<Partition>> result;

        {
            // All localities used by this array
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
                hpx::id_type const locality{localities[p]};

                if(result.find(locality) == result.end())
                {
                    // For this locality, we do not have created a
                    // collection of partitions yet. Create one and
                    // already reserve enough memory.
                    std::vector<Partition> empty_partition_collection{};
                    empty_partition_collection.reserve(max_nr_partitions_per_locality);
                    result[locality] = std::move(empty_partition_collection);
                }

                lue_hpx_assert(result[locality].capacity() > result[locality].size());
                result[locality].push_back(partitions[p]);
            }
        }

        return result;
    }

    template<
        typename Offset,
        typename Shape,
        typename Element>
    using PartitionTuple = std::tuple<Offset, Shape, Element*>;


    template<
        typename Offset,
        typename Shape,
        typename Element>
    using PartitionTuples = std::vector<PartitionTuple<Offset, Shape, Element>>;


    template<
        typename Partition>
    std::tuple<
        PartitionTuples<OffsetT<Partition>, ShapeT<Partition>, ElementT<Partition>>,
        OffsetT<Partition>, ShapeT<Partition>> partition_tuples(
            std::vector<Partition>& partitions)
    {
        using Offset = OffsetT<Partition>;
        using Shape = ShapeT<Partition>;
        using Element = ElementT<Partition>;
        using PartitionServer = typename Partition::Server;
        using PartitionTuple = PartitionTuple<Offset, Shape, Element>;

        // For each partition:
        // - offset: relative to array hyperslab!
        // - shape
        // - buffer
        PartitionTuples<Offset, Shape, Element> partition_tuples{};
        partition_tuples.reserve(partitions.size());

        lue_hpx_assert(!std::empty(partitions));

        // Indices of first and last rows and columns of the block to read.
        // These indices are relative to the array hyperslab
        Index begin_partition_row{std::numeric_limits<Index>::max()};
        Index begin_partition_col{std::numeric_limits<Index>::max()};
        Index end_partition_row{0};  // One past the last row
        Index end_partition_col{0};  // One past the last col

        for(Partition const& partition: partitions)
        {
            auto partition_ptr{detail::ready_component_ptr(partition)};
            PartitionServer& partition_server{*partition_ptr};
            Offset partition_offset{partition_server.offset()};
            Shape partition_shape{partition_server.shape()};
            Element* buffer{partition_server.data().data()};

            partition_tuples.push_back(PartitionTuple{partition_offset, partition_shape, buffer});

            // Keep track of shape of block containing the partitions
            begin_partition_row = std::min(begin_partition_row, std::get<0>(partition_offset));
            begin_partition_col = std::min(begin_partition_col, std::get<1>(partition_offset));
            end_partition_row =
                std::max(end_partition_row, std::get<0>(partition_offset) + std::get<0>(partition_shape));
            end_partition_col =
                std::max(end_partition_col, std::get<1>(partition_offset) + std::get<1>(partition_shape));
        }

        lue_hpx_assert(begin_partition_row <= end_partition_row);
        lue_hpx_assert(begin_partition_col <= end_partition_col);

        Offset const block_offset{
            begin_partition_row,
            begin_partition_col};

        Shape const block_shape{
            end_partition_row - begin_partition_row,
            end_partition_col - begin_partition_col};

        lue_hpx_assert(partition_tuples.size() == partitions.size());

        return std::make_tuple(std::move(partition_tuples), block_offset, block_shape);
    }


    template<
        typename Partition,
        typename Compare>
    std::tuple<
        PartitionTuples<OffsetT<Partition>, ShapeT<Partition>, ElementT<Partition>>,
        OffsetT<Partition>, ShapeT<Partition>> partition_tuples(
            std::vector<Partition>& partitions,
            Compare compare)
    {
        auto [partition_tuples, block_offset, block_shape] = detail::partition_tuples(partitions);

        std::sort(partition_tuples.begin(), partition_tuples.end(), compare);

        return std::make_tuple(std::move(partition_tuples), block_offset, block_shape);
    }


    template<
        typename Subspan>
    void copy(
        Subspan const& subspan,
        typename Subspan::value_type* buffer)
    {
        // Copy elements from subspan to buffer. It is assumed that
        // all values are stored in row-major order.
        lue_hpx_assert(subspan.rank() == 2);

        for(Index r = 0; r < subspan.extent(0); ++r)
        {
            std::copy(
                &subspan(r, 0                ),
                &subspan(r, subspan.extent(1)),
                buffer + r * subspan.extent(1));
        }
    }


    /*!
        @brief      Copy elements from a partition's @a buffer to a
                    block's @a subspan
        @param      buffer Linear array with values
        @param      subspan Subspan into a larger linear array with values
        @return     .
        @exception  .

        The idea is to copy the / all values from the array pointed to
        by @buffer to the right location in the array pointed to by
        @a subspan.
    */
    template<
        typename Subspan>
    void copy(
        typename Subspan::value_type const* buffer,
        Subspan const& subspan)
    {
        // It is assumed that all values are stored in row-major order.
        lue_hpx_assert(subspan.rank() == 2);

        for(Index r = 0; r < subspan.extent(0); ++r)
        {
            std::copy(
                buffer + r       * subspan.extent(1),
                buffer + (r + 1) * subspan.extent(1),
                &subspan(r, 0));
        }
    }


    template<
        Rank rank>
    hdf5::Hyperslab block_hyperslab(
        hdf5::Offset const& array_hyperslab_start,
        Offset<Index, rank> const& block_offset,
        Shape<Count, rank> const& block_shape)
    {
        using Offset = lue::Offset<Index, rank>;

        // Offset of the array hyperslab to the whole array
        Offset const array_hyperslab_offset{
            static_cast<Index>(array_hyperslab_start[0]),
            static_cast<Index>(array_hyperslab_start[1])};

        // Offset of the block relative to the whole array
        Offset const block_offset_within_array{
            std::get<0>(array_hyperslab_offset) + std::get<0>(block_offset),
            std::get<1>(array_hyperslab_offset) + std::get<1>(block_offset)};

        return hyperslab(block_offset_within_array, block_shape);
    }


    template<
        Rank rank>
    hdf5::Hyperslab block_hyperslab(
        hdf5::Offset const& array_hyperslab_start,
        Index const time_step_idx,
        Offset<Index, rank> const& block_offset,
        Shape<Count, rank> const& block_shape)
    {
        using Offset = lue::Offset<Index, rank>;

        // Offset of the array hyperslab to the whole array
        Offset const array_hyperslab_offset{
            static_cast<Index>(array_hyperslab_start[0]),
            static_cast<Index>(array_hyperslab_start[1])};

        // Offset of the block relative to the whole array
        Offset const block_offset_within_array{
            std::get<0>(array_hyperslab_offset) + std::get<0>(block_offset),
            std::get<1>(array_hyperslab_offset) + std::get<1>(block_offset)};

        return hyperslab(block_offset_within_array, block_shape, 0, time_step_idx);
    }


    template<
        Rank rank>
    hdf5::Hyperslab block_hyperslab(
        Index const time_step_idx,
        Offset<Index, rank> const& block_offset,
        Shape<Count, rank> const& block_shape)
    {
        hdf5::Offset array_hyperslab_start(rank, 0);

        return block_hyperslab(array_hyperslab_start, time_step_idx, block_offset, block_shape);
    }

}  // namespace lue::detail
