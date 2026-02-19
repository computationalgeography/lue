#pragma once
#include "lue/framework/core/annotate.hpp"
#include "lue/framework/partitioned_array_decl.hpp"


namespace lue::detail {

    /*!
        @brief      Return an array of the same shape as @a partitions with in each element a
                    future to the number of elements in the input partition

        It is safe to call this function when the partitions are not ready yet. We will
        asynchronously wait for each of them to become ready.
    */
    template<typename Partitions>
    Array<hpx::future<Count>, rank<Partitions>> partition_sizes(Partitions const& partitions)
    {
        using Partition = PartitionT<Partitions>;
        using Shape = ShapeT<Partitions>;
        using PartitionSizes = Array<hpx::future<Count>, rank<Partitions>>;

        Shape const& shape_in_partitions{partitions.shape()};
        PartitionSizes partition_sizes{shape_in_partitions};
        auto const [nr_partitions0, nr_partitions1] = shape_in_partitions;

        for (Index idx0 = 0; idx0 < nr_partitions0; ++idx0)
        {
            for (Index idx1 = 0; idx1 < nr_partitions1; ++idx1)
            {
                // Only after the input partition is ready can we ask for the number of elements
                partition_sizes(idx0, idx1) = hpx::dataflow(
                    hpx::launch::async,

                    [](Partition const& partition) { return partition.nr_elements(); },

                    partitions(idx0, idx1));
            }
        }

        return partition_sizes;
    }


    template<typename Partitions>
    Array<hpx::shared_future<ShapeT<Partitions>>, rank<Partitions>> partition_shapes(
        Partitions const& partitions)
    {
        using Shape = ShapeT<Partitions>;
        using PartitionShapes = Array<hpx::shared_future<Shape>, rank<Partitions>>;

        Shape const& shape_in_partitions{partitions.shape()};
        PartitionShapes partition_shapes{shape_in_partitions};
        auto const [nr_partitions0, nr_partitions1] = shape_in_partitions;

        for (Index idx0 = 0; idx0 < nr_partitions0; ++idx0)
        {
            for (Index idx1 = 0; idx1 < nr_partitions1; ++idx1)
            {
                partition_shapes(idx0, idx1) = partitions(idx0, idx1).shape();
            }
        }

        return partition_shapes;
    }


    template<typename Partitions, typename PartitionShapes>
    Array<hpx::shared_future<DataT<PartitionT<Partitions>>>, rank<Partitions>> partition_data(
        Partitions const& partitions, PartitionShapes const& partition_shapes)
    {
        using Partition = PartitionT<Partitions>;
        Rank const rank{lue::rank<Partition>};
        using PartitionData = DataT<Partition>;
        using InputPartitionsData = Array<hpx::shared_future<PartitionData>, rank>;
        using Shape = ShapeT<Partitions>;

        Shape const& shape_in_partitions{partitions.shape()};
        InputPartitionsData data{shape_in_partitions};

        using Slice = SliceT<Partition>;
        using Slices = SlicesT<Partition>;

        // North-west corner partition: get south-east corner element
        data(0, 0) = partition_shapes(0, 0).then(
            hpx::unwrapping(

                [input_partition = partitions(0, 0)](Shape const& partition_shape)
                {
                    AnnotateFunction annotation{"partition_data"};

                    auto const [nr_elements0, nr_elements1] = partition_shape;

                    return input_partition.slice(
                        Slices{
                            {Slice{nr_elements0 - 1, nr_elements0}, Slice{nr_elements1 - 1, nr_elements1}}});
                }

                ));

        // North partition: get south side elements
        data(0, 1) = partition_shapes(0, 1).then(
            hpx::unwrapping(

                [input_partition = partitions(0, 1)](Shape const& partition_shape)
                {
                    AnnotateFunction annotation{"partition_data"};

                    auto const [nr_elements0, nr_elements1] = partition_shape;

                    return input_partition.slice(
                        Slices{{Slice{nr_elements0 - 1, nr_elements0}, Slice{0, nr_elements1}}});
                }

                ));

        // North-east partition: get south-west corner elements
        data(0, 2) = partition_shapes(0, 2).then(
            hpx::unwrapping(

                [input_partition = partitions(0, 2)](Shape const& partition_shape)
                {
                    AnnotateFunction annotation{"partition_data"};

                    auto const [nr_elements0, nr_elements1] = partition_shape;

                    return input_partition.slice(
                        Slices{{Slice{nr_elements0 - 1, nr_elements0}, Slice{0, 1}}});
                }

                ));

        // West partition: get east side elements
        data(1, 0) = partition_shapes(1, 0).then(
            hpx::unwrapping(

                [input_partition = partitions(1, 0)](Shape const& partition_shape)
                {
                    AnnotateFunction annotation{"partition_data"};

                    auto const [nr_elements0, nr_elements1] = partition_shape;

                    return input_partition.slice(
                        Slices{{Slice{0, nr_elements0}, Slice{nr_elements1 - 1, nr_elements1}}});
                }

                ));

        // Center partition: get all elements
        data(1, 1) = partitions(1, 1).data();

        // East partition: get west side elements
        data(1, 2) = partition_shapes(1, 2).then(
            hpx::unwrapping(

                [input_partition = partitions(1, 2)](Shape const& partition_shape)
                {
                    AnnotateFunction annotation{"partition_data"};

                    auto const [nr_elements0, nr_elements1] = partition_shape;

                    return input_partition.slice(Slices{{Slice{0, nr_elements0}, Slice{0, 1}}});
                }

                ));

        // South-west partition: get north-east corner elements
        data(2, 0) = partition_shapes(2, 0).then(
            hpx::unwrapping(

                [input_partition = partitions(2, 0)](Shape const& partition_shape)
                {
                    AnnotateFunction annotation{"partition_data"};

                    auto const [nr_elements0, nr_elements1] = partition_shape;

                    return input_partition.slice(
                        Slices{{Slice{0, 1}, Slice{nr_elements1 - 1, nr_elements1}}});
                }

                ));

        // South partition: get north side elements
        data(2, 1) = partition_shapes(2, 1).then(
            hpx::unwrapping(

                [input_partition = partitions(2, 1)](Shape const& partition_shape)
                {
                    AnnotateFunction annotation{"partition_data"};

                    auto const [nr_elements0, nr_elements1] = partition_shape;

                    return input_partition.slice(Slices{{Slice{0, 1}, Slice{0, nr_elements1}}});
                }

                ));

        // South-east partition: get north-west corner element
        data(2, 2) = partitions(2, 2).slice(Slices{{Slice{0, 1}, Slice{0, 1}}});

        lue_hpx_assert(all_are_valid(data));

        return data;
    }

}  // namespace lue::detail
