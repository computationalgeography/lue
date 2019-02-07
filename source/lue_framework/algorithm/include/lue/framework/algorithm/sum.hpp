#pragma once
// #include "lue/framework/core/array_partition_data.hpp"
#include <hpx/dataflow.hpp>
// #include <boost/range/combine.hpp>


namespace lue {

template<
    typename Value,
    std::size_t rank,
    template<typename, std::size_t> typename Data,
    template<typename, typename> typename PartitionedArray
>
// hpx::future<typename PartitionedArray::ValueType> sum(
hpx::future<Value> sum(
    PartitionedArray<Value, Data<Value, rank>>& array)
{
    // Attach a continuation to each partition that sums all elements in
    // that partition

    // Store these results locally. This doesn't scale to large numbers
    // of partitions, so revisit when this becomes a problem.

    // Create an array with a shape that corresponds with the shape in
    // partitions of the partitioned array

    // TODO Deduce type from array instead of using a lue type directly
    // using ValueType = typename PartitionedArray::ValueType;
    // std::size_t const rank = 0;
    // using Data = ArrayPartitionData<ValueType, rank>;

    // TODO Create distribution policy which distributes partitions
    //      based on an existing distribution
    //      Below we want to assign to each partition

    using PartitionSums = PartitionedArray<Value, Data<Value, rank>>;
    PartitionSums partition_sums{array.partitions().shape()};

    assert(array.nr_partitions() == partition_sums.nr_partitions());
    assert(partition_sums.nr_partitions() == partition_sums.nr_elements());

    // TODO Hier verder
    // - Configure distribution policy to create more partitions than
    //   there are localities
    // - Make the creation of components optional when instantiating a
    //   partitioned array? We want to create and assign components
    //   ourselves here.

    auto partition_sum_it = partition_sums.begin();

    for(auto partition_it = array.partitions().begin();
            partition_it != array.end(); ++partition_it, ++partition_sum_it) {

    //     // Assign to *partition_sum_it the future to the result of
    //     // calculating the sum of *partition_id

    //     *partition_sum_it = (*partition_it).then(
    //         [](
    //             auto& partition)
    //         {
    //             return hpx::make_ready_future<Value>(3);
    //         }
    //     );

    }





    //     // *partition_sum_it = hpx::dataflow(
    //     //     hpx::launch::async,
    //     //     hpx::util::unwrapping(
    //     //         [](
    //     //             auto const& partition)
    //     //         {
    //     //             // Partition is the ID of the partition component

    //     //             

    //     //             std::string i = partition;

    //     //             return Value{3};
    //     //         }
    //     //     ),
    //     //     *partition_it);
    // }


    // for(auto& partition: array) {
        // partition = hpx::dataflow(
        //     hpx::launch::async,
        //     hpx::unwrapping(
        //         [](
        //             auto const& partition,
        //             auto const& value)
        //         {
        //             return partition
        //         }
        //     ),
        //     partition, value);
    // }





    // Attach a continuation to each partition sum that sums all
    // partition sums


    // using ValueType = typename PartitionedArray::ValueType;

    return hpx::make_ready_future<Value>(9);
}

}  // namespace lue
