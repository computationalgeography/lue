#pragma once
#include "lue/framework/core/indices.hpp"
#include "lue/framework/core/array_partition_definition.hpp"
#include "lue/assert.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <vector>
#include <iostream>


namespace lue {

template<
    typename Index,
    std::size_t rank>
Shape<Index, rank> shape_in_partitions(
    Shape<Index, rank> const& area_shape,
    Shape<Index, rank> const& partition_shape)
{
    Shape<Index, rank> result;

    std::transform(
        area_shape.begin(), area_shape.end(), partition_shape.begin(),
        result.begin(),
        [](Index const area_extent, Index const partition_extent)
        {
            return
                static_cast<Index>(std::ceil(
                    double(area_extent) / double(partition_extent)));
        });

    return result;
}


template<
    typename Index,
    std::size_t rank>
std::size_t nr_partitions(
    Shape<Index, rank> const& area_shape,
    Shape<Index, rank> const& partition_shape)
{
    auto const shape = shape_in_partitions(area_shape, partition_shape);

    return std::accumulate(
        shape.begin(), shape.end(), std::size_t{1},
        std::multiplies<std::size_t>());
}


/*!
    @brief      Return a partition shape that can be used to partition
                @a array_shape in at least @a min_nr_partitions partitions
    @param      array_shape Shape of the array to partition. This shape
                must not be empty.
    @param      min_nr_partitions Minimum number of partitions
                requested. This number must lie in the range [1,
                nr_elements(@a shape)].

    When partitioning the array using the partition shape returned,
    the partitions at the borders of the array may not be fully part of
    the array. In that case, those partitions will have to be resized
    to match the array's extent.
*/
template<
    typename Index,
    std::size_t rank>
Shape<Index, rank> max_partition_shape(
    Shape<Index, rank> const& array_shape,
    Index const min_nr_partitions)
{
    static_assert(rank > 0);

    lue_assert(nr_elements(array_shape) > 0);
    lue_assert(min_nr_partitions > 0);
    lue_assert(min_nr_partitions <= nr_elements(array_shape));

    Shape<Index, rank> partition_shape{array_shape};

    Index const nr_partitions_per_dimension =
        static_cast<Index>(std::ceil(std::pow(min_nr_partitions, 1.0 / rank)));

    Index nr_partitions = 0;

    for(auto& extent: partition_shape) {
        if(nr_partitions < min_nr_partitions) {
            extent /= nr_partitions_per_dimension;
            nr_partitions += nr_partitions_per_dimension;
        }
    }

    lue_assert(nr_elements(partition_shape) > 0);
    lue_assert(nr_elements(shape_in_partitions(array_shape, partition_shape)) >=
        min_nr_partitions);

    return partition_shape;
}


template<
    typename Index,
    std::size_t rank>
Indices<Index, rank> linear_to_shape_index(
    Shape<Index, rank> const& shape,
    Index idx)
{
    static_assert(rank > 0);
    lue_assert(
        idx < std::accumulate(
            shape.begin(), shape.end(), Index{1}, std::multiplies<Index>()));

    // Given a shape and a linear index, return the corresponding cell
    // indices along each dimension

    Indices<Index, rank> result;

    {
        auto result_ptr = result.begin();

        // Iterate over all dimensions
        for(auto shape_ptr = shape.begin(); shape_ptr != shape.end();
                ++shape_ptr) {

            // Determine the number of cells represented by a single increment
            // along the current dimension
            auto const nr_cells = std::accumulate(
                shape_ptr + 1, shape.end(), Index{1}, std::multiplies<Index>());

            auto& dimension_index = *result_ptr;

            dimension_index = idx / nr_cells;  // Integer division: floors
            idx -= dimension_index * nr_cells;

            ++result_ptr;
        }
    }

    return result;
}


template<
    typename Index,
    std::size_t rank>
ArrayPartitionDefinition<Index, rank> partition(
    Shape<Index, rank> const& area_shape,
    Shape<Index, rank> const& partition_shape,
    Shape<Index, rank> const& shape_in_partitions,
    std::size_t const idx)
{
    typename ArrayPartitionDefinition<Index, rank>::Start start;

    {
        // Determine indices of cell at start of partition
        // Convert between shape_in_partitions to area_shape indices
        auto const indices = linear_to_shape_index(shape_in_partitions, idx);

        std::transform(
            indices.begin(), indices.end(), partition_shape.begin(),
            start.begin(),
            [](
                Index const partition_dimension_index,
                Index const partition_dimension_extent)
            {
                return partition_dimension_index * partition_dimension_extent;
            });
    }

    ArrayPartitionDefinition<Index, rank> result{start, partition_shape};

    // Determine final shape of partition, taking into account that the
    // partition must not extent beyond the area's shape
    for(std::size_t i = 0; i < rank; ++i) {
        auto const extent =
            std::min(result.start()[i] + result.shape()[i], area_shape[i]);

        result.shape()[i] = extent - result.start()[i];
    }

    return result;
}


template<
    typename Index,
    std::size_t rank>
Shape<Index, rank> clamp_area_shape(
    Shape<Index, rank> const& area_shape,
    Shape<Index, rank> const& partition_shape)
{
    // Iterate over each extent and divide the area extent by the partition
    // extent (rounded up). Create a new area shape in which along each
    // extent a whole number of partitions fit.
    Shape<Index, rank> result;

    std::transform(
        area_shape.begin(), area_shape.end(), partition_shape.begin(),
        result.begin(),
        [](Index const area_extent, Index const partition_extent)
        {
            return
                static_cast<Index>(std::ceil(
                    double(area_extent) / double(partition_extent))) *
                partition_extent;
        });

    return result;
}


template<
    typename Index,
    std::size_t rank>
std::vector<ArrayPartitionDefinition<Index, rank>> partitions(
    Shape<Index, rank> const& area_shape,
    Shape<Index, rank> const& partition_shape,
    std::size_t const nr_localities,
    std::uint32_t const locality_id)
{
    lue_assert(nr_localities > 0);
    lue_assert(locality_id < nr_localities);

    auto const shape_in_partitions_ =
        shape_in_partitions(area_shape, partition_shape);
    auto const nr_partitions = std::accumulate(
        shape_in_partitions_.begin(), shape_in_partitions_.end(),
        std::size_t(1), std::multiplies<std::size_t>());

    std::vector<ArrayPartitionDefinition<Index, rank>> result;

    // TODO
    // Pick partitions according to the Hilbert curve. This way, partitions
    // within a locality are close to each other.

    if(nr_partitions <= nr_localities) {
        // The first nr_partitions localities will get a single partition. The
        // rest will get none. This is a waste of resources. We seem
        // to have more localities than we need.
        if(locality_id < nr_partitions) {
            result.emplace_back(
                partition(area_shape, partition_shape, shape_in_partitions_,
                    locality_id));
        }
    }
    else {
        // Try to assign an equal amount of partitions to each locality. This
        // is not always possible. The last n localities might have one
        // partition less than ones before that.
        auto const max_nr_partitions_per_locality =
            static_cast<std::size_t>(
                std::ceil(double(nr_partitions) / double(nr_localities)));
        auto const nr_localities_with_less_partitions =
            (nr_localities * max_nr_partitions_per_locality) - nr_partitions;
        std::uint32_t const first_locality_with_less_partitions =
            nr_localities - nr_localities_with_less_partitions;

        std::size_t begin_partition_idx;
        std::size_t end_partition_idx;

        // Determine indices of partitions to assign to current locality
        {
            if(locality_id < first_locality_with_less_partitions) {
                // Current locality is one of those that get the regular
                // amount of partitions assigned
                begin_partition_idx =
                    locality_id * max_nr_partitions_per_locality;
                end_partition_idx =
                    begin_partition_idx + max_nr_partitions_per_locality;
            }
            else {
                // Current locality is one of those that get one partition
                // less assigned
                begin_partition_idx =
                    first_locality_with_less_partitions *
                    max_nr_partitions_per_locality;
                begin_partition_idx +=
                    (locality_id - first_locality_with_less_partitions) *
                    (max_nr_partitions_per_locality - 1);
                end_partition_idx =
                    begin_partition_idx + (max_nr_partitions_per_locality - 1);
            }
        }

        // Iterate over partition indices and obtain actual partition
        // definitions
        {
            auto const nr_partitions = end_partition_idx - begin_partition_idx;
            result.reserve(nr_partitions);

            for(std::uint32_t partition_id = begin_partition_idx;
                    partition_id < end_partition_idx; ++partition_id) {

                result.emplace_back(
                    partition(
                        area_shape, partition_shape, shape_in_partitions_,
                        partition_id));

            }
        }
    }

    return result;
}

}  // namespace lue
