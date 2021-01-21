#pragma once
#include "lue/framework/core/component/partitioned_array.hpp"
// #include "lue/framework/core/type_traits.hpp"


namespace lue {
namespace detail {

template<
    typename Partitions>
Array<hpx::shared_future<ShapeT<Partitions>>, rank<Partitions>> partition_shapes(
    Partitions const& partitions)
{
    using Shape = ShapeT<Partitions>;
    using PartitionShapes = Array<hpx::shared_future<Shape>, rank<Partitions>>;

    Shape const& shape_in_partitions{partitions.shape()};
    PartitionShapes partition_shapes{shape_in_partitions};
    auto const [nr_partitions0, nr_partitions1] = shape_in_partitions;

    for(Index idx0 = 0; idx0 < nr_partitions0; ++idx0) {
        for(Index idx1 = 0; idx1 < nr_partitions1; ++idx1)
        {
            partition_shapes(idx0, idx1) = partitions(idx0, idx1).shape();
        }
    }

    return partition_shapes;
}


template<
    typename Partitions,
    typename PartitionShapes>
Array<hpx::shared_future<DataT<PartitionT<Partitions>>>, rank<Partitions>> partition_data(
    Partitions const& partitions,
    PartitionShapes const& partition_shapes)
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
    data(0, 0) = partition_shapes(0, 0).then(hpx::util::unwrapping(

            [flow_direction_partition=partitions(0, 0)](
                Shape const& partition_shape)
            {
                auto const [nr_elements0, nr_elements1] = partition_shape;

                return flow_direction_partition.slice(
                    Slices{{
                        Slice{nr_elements0 - 1, nr_elements0},
                        Slice{nr_elements1 - 1, nr_elements1}
                    }});
            }

        ));

    // North partition: get south side elements
    data(0, 1) = partition_shapes(0, 1).then(hpx::util::unwrapping(

            [input_partition=partitions(0, 1)](
                Shape const& partition_shape)
            {
                auto const [nr_elements0, nr_elements1] = partition_shape;

                return input_partition.slice(
                    Slices{{
                        Slice{nr_elements0 - 1, nr_elements0},
                        Slice{0, nr_elements1}
                    }});
            }

        ));

    // North-east partition: get south-west corner elements
    data(0, 2) = partition_shapes(0, 2).then(hpx::util::unwrapping(

            [input_partition=partitions(0, 2)](
                Shape const& partition_shape)
            {
                auto const [nr_elements0, nr_elements1] = partition_shape;

                return input_partition.slice(
                    Slices{{
                        Slice{nr_elements0 - 1, nr_elements0},
                        Slice{0, 1}
                    }});
            }

        ));

    // West partition: get east side elements
    data(1, 0) = partition_shapes(1, 0).then(hpx::util::unwrapping(

            [input_partition=partitions(1, 0)](
                Shape const& partition_shape)
            {
                auto const [nr_elements0, nr_elements1] = partition_shape;

                return input_partition.slice(
                    Slices{{
                        Slice{0, nr_elements0},
                        Slice{nr_elements1 - 1, nr_elements1}
                    }});
            }

        ));

    // Center partition: get all elements
    data(1, 1) = partitions(1, 1).data();

    // East partition: get west side elements
    data(1, 2) = partition_shapes(1, 2).then(hpx::util::unwrapping(

            [input_partition=partitions(1, 2)](
                Shape const& partition_shape)
            {
                auto const [nr_elements0, nr_elements1] = partition_shape;

                return input_partition.slice(
                    Slices{{
                        Slice{0, nr_elements0},
                        Slice{0, 1}
                    }});
            }

        ));

    // South-west partition: get north-east corner elements
    data(2, 0) = partition_shapes(2, 0).then(hpx::util::unwrapping(

            [input_partition=partitions(2, 0)](
                Shape const& partition_shape)
            {
                auto const [nr_elements0, nr_elements1] = partition_shape;

                return input_partition.slice(
                    Slices{{
                        Slice{0, 1},
                        Slice{nr_elements1 - 1, nr_elements1}
                    }});
            }

        ));

    // South partition: get north side elements
    data(2, 1) = partition_shapes(2, 1).then(hpx::util::unwrapping(

            [input_partition=partitions(2, 1)](
                Shape const& partition_shape)
            {
                auto const [nr_elements0, nr_elements1] = partition_shape;

                return input_partition.slice(
                    Slices{{
                        Slice{0, 1},
                        Slice{0, nr_elements1}
                    }});
            }

        ));

    // South-east partition: get north-west corner element
    data(2, 2) = partitions(2, 2).slice(
        Slices{{
            Slice{0, 1},
            Slice{0, 1}
        }});

    lue_assert(all_are_valid(data));

    return data;
}

}  // namespace detail
}  // namespace lue
