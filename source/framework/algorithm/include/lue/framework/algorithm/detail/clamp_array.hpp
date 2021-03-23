#pragma once
#include "lue/framework/core/annotate.hpp"
#include "lue/framework/core/component/partitioned_array.hpp"
#include "lue/framework/core/array_partition_visitor.hpp"


namespace lue::detail {

    template<
        typename Partitions>
    class ShrinkVisitor:
        public PartitionVisitor<Partitions>
    {

    private:

        using Base = PartitionVisitor<Partitions>;

    public:

        ShrinkVisitor(
            Partitions& partitions,
            ShapeT<Partitions> const& new_shape):

            Base{partitions},
            _new_shape{new_shape}

        {
        }

        void operator()()
        {
            using Partition = PartitionT<Partitions>;
            using Shape = ShapeT<Partitions>;

            Partition& partition = this->partition();

            // Only once the partition is ready we can manipulate it
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
            partition = partition.then(
                    [new_shape=this->_new_shape](
                        Partition&& partition)
                    {
                        // Once the current shape is obtained we can
                        // reshape the partition
                        return partition.shape().then(
                            hpx::util::unwrapping(
                                [new_shape, partition](
                                    Shape current_shape) mutable
                                {
                                    AnnotateFunction annotation{"shrink_partition"};

                                    Count const rank =
                                        static_cast<Count>(lue::rank<Partitions>);

                                    // Update current shape given requested shape
                                    for(Index i = 0; i < rank; ++i) {
                                        current_shape[i] =
                                            std::min(current_shape[i], new_shape[i]);
                                    }

                                    // FIXME Blocks current thread... Is this a problem?
                                    //     Guess not, we are doing useful stuff.
                                    partition.reshape(current_shape).wait();

                                    return partition;
                                }
                            ));
                    }
                );
#pragma GCC diagnostic pop
        }

    private:

        //! New shape to use for partitions visited
        ShapeT<Partitions> const _new_shape;

    };


    template<
        typename Element,
        Rank rank>
    void shrink_partitions(
        PartitionsT<PartitionedArray<Element, rank>>& partitions,
        ShapeT<PartitionedArray<Element, rank>> const& begin_indices,
        ShapeT<PartitionedArray<Element, rank>> const& end_indices,
        ShapeT<PartitionedArray<Element, rank>> const& new_shape)
    {
        // Iterate over all partitions and resize each of them.
        // Array shape is not changed! It is assumed that the shape of
        // the partitioned array after resizing corresponds with the cached
        // array shape (_shape).
        // The new_shape passed in is taken to be the max size to use. If
        // one or more dimensions of a partition is smaller than the extents
        // in new_shape, then those dimensions of the partition are not
        // resized. Partition extents are made smaller, not larger.

        ShrinkVisitor shrink{partitions, new_shape};

        visit_array(begin_indices, end_indices, shrink);
    }


    template<
        typename Element,
        Rank rank>
    void clamp_array_shrink(
        PartitionsT<PartitionedArray<Element, rank>>& partitions,
        ShapeT<PartitionedArray<Element, rank>> const& array_shape,
        ShapeT<PartitionedArray<Element, rank>> const& shape_in_partitions,
        ShapeT<PartitionedArray<Element, rank>> const& max_partition_shape)
    {
        // Shrink partitions at the border of the array. They might be too large.
        // This happens when the extents of the array are not a multiple of the
        // corresponding extents in max_partition_shape.

        // Resize partitions at the sides of the array that are too large

        // Offset to iterate over partition along one of the dimensions
        // Index partition_idx_offset;

        using Array = PartitionedArray<Element, rank>;
        using Shape = ShapeT<Array>;

        // Begin and end indices of all partitions in the array
        Shape begin_indices;
        begin_indices.fill(0);
        Shape const end_indices{shape_in_partitions};

        // Iterate over all dimensions
        for(Rank d = 0; d < rank; ++d)
        {
            // Extent of current dimension in partitioned array
            auto const array_extent = array_shape[d];

            // Maximum extent of current dimension in partitioned array, given
            // the number of partitions and the max extent
            auto const max_partitions_extent =
                shape_in_partitions[d] * max_partition_shape[d];

            // Excess cells along current dimension in partitioned
            // array. These cells need to be removed.
            auto const excess_cells = max_partitions_extent - array_extent;

            // We are assuming here that only the last partition needs to
            // be resized. All other partitions must be positioned within
            // the array's extent.
            lue_hpx_assert(excess_cells < max_partition_shape[d]);

            if(excess_cells > 0)
            {
                // Resize current dimension of all partitions at currently
                // considered side of array
                // These partitions form a hyperslab of the array. We can
                // select this slab using start indices and end indices along
                // all dimensions. For the current dimension we set these
                // to the same value: the index of the last partition.
                //
                // In case of the other dimensions we set the start
                // indices to zero and the end indices to the last index
                // along each of these dimensions.

                Shape begin_indices_hyperslab{begin_indices};
                Shape end_indices_hyperslab{end_indices};

                begin_indices_hyperslab[d] = shape_in_partitions[d] - 1;
                end_indices_hyperslab[d] = shape_in_partitions[d];

                Shape partition_shape{max_partition_shape};
                partition_shape[d] -= excess_cells;

                shrink_partitions<Element, rank>(
                    partitions,
                    begin_indices_hyperslab, end_indices_hyperslab,
                    partition_shape);
            }
        }
    }


    template<
        typename Partitions>
    class ResizeVisitor:
        public PartitionVisitor<Partitions>
    {

    private:

        using Base = PartitionVisitor<Partitions>;

    public:

        ResizeVisitor(
            Partitions& partitions,
            Index const dimension_idx,
            Count const new_size):

            Base{partitions},
            _dimension_idx{dimension_idx},
            _new_size{new_size}

        {
            lue_hpx_assert(_dimension_idx >= 0);
            lue_hpx_assert(_dimension_idx < static_cast<Index>(rank<Partitions>));
        }

        void operator()()
        {
            using Partition = PartitionT<Partitions>;
            using Shape = ShapeT<Partitions>;

            Partition& partition = this->partition();

            // Only once the partition is ready we can manipulate it
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
            partition = partition.then(
                    [dimension_idx=this->_dimension_idx, new_size=this->_new_size](
                        Partition&& partition)
                    {
                        // Once the current shape is obtained we can
                        // reshape the partition
                        return partition.shape().then(
                            hpx::util::unwrapping(
                                [dimension_idx, new_size, partition](
                                    Shape current_shape) mutable
                                {
                                    AnnotateFunction annotation{"resize_partition"};

                                    current_shape[dimension_idx] = new_size;

                                    // FIXME Blocks current thread... Is this a problem?
                                    //     Guess not, we are doing useful stuff.
                                    partition.reshape(current_shape).wait();

                                    return partition;
                                }
                            ));
                    }
                );
#pragma GCC diagnostic pop
        }

    private:

        Index          _dimension_idx;

        Count          _new_size;

    };


    template<
        typename Element,
        Rank rank>
    void resize_partitions(
        PartitionsT<PartitionedArray<Element, rank>>& partitions,
        ShapeT<PartitionedArray<Element, rank>> const& begin_indices,
        ShapeT<PartitionedArray<Element, rank>> const& end_indices,
        Index const dimension_idx,
        Count const new_size)
    {
        // Iterate over all partitions and resize them.
        // Array shape is not changed! It is assumed that the shape of
        // the partitioned array after resizing corresponds with the cached
        // array shape (_shape).
        ResizeVisitor resize{partitions, dimension_idx, new_size};

        visit_array(begin_indices, end_indices, resize);
    }


    template<
        typename Element,
        Rank rank>
    void clamp_array_merge(
        Localities<rank>& localities,
        PartitionsT<PartitionedArray<Element, rank>>& partitions,
        ShapeT<PartitionedArray<Element, rank>> const& array_shape,
        ShapeT<PartitionedArray<Element, rank>> const& shape_in_partitions,
        ShapeT<PartitionedArray<Element, rank>> const& max_partition_shape)
    {
        // Merge partitions at the border of the array. They might be too
        // large (overflow). This happens when the extents of the array are
        // not a multiple of the corresponding extents in max_partition_shape.

        using Array = PartitionedArray<Element, rank>;
        using Shape = ShapeT<Array>;

        // Begin and end indices of all partitions in the array
        Shape begin_indices;
        begin_indices.fill(0);
        Shape end_indices{shape_in_partitions};

        // Iterate over all dimensions
        for(Rank d = 0; d < rank; ++d)
        {
            // Extent of current dimension in partitioned array
            auto const array_extent = array_shape[d];

            // Maximum extent of current dimension in partitioned array, given
            // the number of partitions and the max extent
            auto const max_partitions_extent =
                shape_in_partitions[d] * max_partition_shape[d];

            // Excess cells along current dimension in partitioned
            // array. These cells need to be removed.
            auto const excess_cells = max_partitions_extent - array_extent;

            // We are assuming here that only the last partition needs to
            // be resized. All other partitions must be positioned within
            // the array's extent.
            lue_hpx_assert(excess_cells < max_partition_shape[d]);

            if(excess_cells > 0)
            {
                if(shape_in_partitions[d] == 1)
                {
                    // Only one partition along the current dimension:
                    // shrink it

                    // Resize current dimension of all partitions at currently
                    // considered side of array
                    // These partitions form a hyperslab of the array. We can
                    // select this slab using start indices and end indices along
                    // all dimensions. For the current dimension we set these
                    // to the same value: the index of the last partition.

                    // In case of the other dimensions we set the start
                    // indices to zero and the end indices to the last index
                    // along each of these dimensions.

                    Shape begin_indices_hyperslab{begin_indices};
                    Shape end_indices_hyperslab{end_indices};

                    begin_indices_hyperslab[d] = shape_in_partitions[d] - 1;
                    end_indices_hyperslab[d] = shape_in_partitions[d];

                    Shape partition_shape{max_partition_shape};
                    partition_shape[d] -= excess_cells;

                    shrink_partitions<Element, rank>(
                        partitions,
                        begin_indices_hyperslab, end_indices_hyperslab,
                        partition_shape);
                }
                else
                {
                    // Multiple partitions along the current dimension:
                    // get rid of the last one and enlarge the neighbouring
                    // partition (along the current dimension)

                    // We can select the relevant partitions using start
                    // indices and end indices along all dimensions. For
                    // the current dimension we set these to the same value:
                    // the index of the last partitions.
                    // In case of the other dimensions we set the start
                    // indices to zero and the end indices to the last index
                    // along each of these dimensions.

                    Shape begin_indices_hyperslab{begin_indices};
                    Shape end_indices_hyperslab{end_indices};

                    /// // Remove the last partitions, along the current dimension
                    /// begin_indices_hyperslab[d] = shape_in_partitions[d] - 1;
                    /// end_indices_hyperslab[d] = shape_in_partitions[d];

                    // Resize the one before the last partitions (which just
                    // got erased), along the current dimension
                    begin_indices_hyperslab[d] = shape_in_partitions[d] - 2;
                    end_indices_hyperslab[d] = shape_in_partitions[d] - 1;

                    Shape partition_shape{max_partition_shape};
                    partition_shape[d] += max_partition_shape[d] - excess_cells;

                    resize_partitions<Element, rank>(
                        partitions,
                        begin_indices_hyperslab, end_indices_hyperslab,
                        d, partition_shape[d]);

                    localities.erase(d, shape_in_partitions[d] - 1, shape_in_partitions[d]);
                    partitions.erase(d, shape_in_partitions[d] - 1, shape_in_partitions[d]);

                    end_indices[d] -= 1;
                }
            }
        }
    }

}  // namespace lue::detail
