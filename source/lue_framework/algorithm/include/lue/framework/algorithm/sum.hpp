#pragma once
#include "lue/framework/core/array_partition_data.hpp"
// #include "lue/framework/core/component/partitioned_array.hpp"
#include "lue/framework/core/component/array_partition.hpp"
// #include <hpx/dataflow.hpp>
#include <hpx/hpx.hpp>
// #include <boost/range/combine.hpp>


namespace lue {

/*!
    @brief      Return the sum of values in an array partition
    @param      partition Client of array partition component
    @return     Array partition with a sum in it
*/
template<
    typename Value,
    std::size_t rank,
    template<typename, std::size_t> typename Data,
    template<typename, typename> typename ArrayPartition>
static ArrayPartition<Value, Data<Value, rank>> sum_partition(
    ArrayPartition<Value, Data<Value, rank>> const& partition)
{
    // TODO Assert the current locality equals the one of the server
    // auto hpx::naming::get_locality_id_from_id(r.first);

    // Retrieve the partition data
    hpx::shared_future<Data<Value, rank>> partition_data =
        partition.data();

    // Once the data has arrived, sum the values
    hpx::future<Data<Value, rank>> sum = partition_data.then(
        hpx::util::unwrapping(
            [](Data<Value, rank> const& partition_data)
        {
            typename Data<Value, rank>::Shape shape;

            // Aggregate nD array partition to nD array partition
            // containing a single value
            std::fill(shape.begin(), shape.end(), 1);

            Value result = std::accumulate(
                partition_data.data(),
                partition_data.data() + partition_data.size());

            return Data<Value, rank>{shape, result};
        })
    );

    // Once the sum has been calculated, create a component containing
    // the result, on the same locality as the summed partition
    return sum.then(
        hpx::util::unwrapping(
            [partition](Data<Value, rank>&& sum_data)
        {
            assert(sum_data.size() == 1);

            return ArrayPartition<Value, Data<Value, rank>>(
                partition.get_id(), sum_data);
        })
    );
}


client::ArrayPartition<std::int32_t, ArrayPartitionData<std::int32_t, 1>>
    sum_partition2(
        client::ArrayPartition<
            std::int32_t,
            ArrayPartitionData<std::int32_t, 1>> const& partition)
{
    using Value = std::int32_t;
    std::size_t const rank = 1;
    using Data = ArrayPartitionData<Value, rank>;
    using ArrayPartition = client::ArrayPartition<Value, Data>;


    // TODO Assert the current locality equals the one of the server
    // auto hpx::naming::get_locality_id_from_id(r.first);

    // Retrieve the partition data
    hpx::shared_future<Data> partition_data = partition.data();

    // Once the data has arrived, sum the values
    hpx::future<Data> sum = partition_data.then(
        hpx::util::unwrapping(
            [](Data const& partition_data)
        {
            typename Data::Shape shape;

            // Aggregate nD array partition to nD array partition
            // containing a single value
            std::fill(shape.begin(), shape.end(), 1);

            Value result = std::accumulate(
                partition_data.data(),
                partition_data.data() + partition_data.size(), 0);

            return Data{shape, result};
        })
    );

    // Once the sum has been calculated, create a component containing
    // the result, on the same locality as the summed partition
    return sum.then(
        hpx::util::unwrapping(
            [partition](Data&& sum_data)
        {
            assert(sum_data.size() == 1);

            return ArrayPartition(partition.get_id(), sum_data);
        })
    );
}


// class hpx::actions::basic_action_impl<
//     lue::client::ArrayPartition<int, lue::ArrayPartitionData<int, 1> > (**)(
//         const lue::client::ArrayPartition<int, lue::ArrayPartitionData<int, 1> >&),
//     lue::client::ArrayPartition<int, lue::ArrayPartitionData<int, 1> > (**)(
//         const lue::client::ArrayPartition<int, lue::ArrayPartitionData<int, 1> >&),
//     (& lue::meh),
//     SumPartitionAction
// >

}  // namespace lue

// auto meh = lue::sum_partition<
//     std::int32_t,
//     1,
//     lue::ArrayPartitionData,
//     lue::client::ArrayPartition
// >;


// auto meh = [](
//         lue::client::ArrayPartition<
//             std::int32_t,
//             lue::ArrayPartitionData<std::int32_t, 1>> const& partition)
//     {
//         return lue::sum_partition(partition);
//     };
// 
// 
// HPX_PLAIN_ACTION(meh, SumPartitionAction);

HPX_PLAIN_ACTION(lue::sum_partition2, SumPartitionAction);


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

    // Create an array with a shape that corresponds with the shape in
    // partitions of the partitioned array

    // TODO Create distribution policy which distributes partitions
    //      based on an existing distribution
    //      Below we want to assign to each partition
    //      Maybe just don't distribute partitions but make it possible
    //      to assign partitions later on

    // How to create a partitioned array based on components that are
    // created on the fly. The partitioned array should not create these
    // components itself. It should digest them.

    using Sums = PartitionedArray<Value, Data<Value, rank>>;
    // Sums partition_sums{array.partitions().shape()};

    typename Sums::Partitions sums_partitions{array.partitions().shape()};
    SumPartitionAction sum_partition;

    for(std::size_t p = 0; p < array.nr_partitions(); ++p) {

        // // Execute action on the locality of the current partition
        // auto Op = hpx::util::bind(sum_partition, array.locality(), _1);

        // sums_partitions.partitions()[p] = hpx::dataflow(
        //     hpx::launch::async,
        //     Op,
        //     array.partitions()[p]);

        sums_partitions[p] = hpx::dataflow(
            hpx::launch::async,
            sum_partition,
            hpx::find_here(),  // TODO
            array.partitions()[p]);


        // array.partitions()[p].then(sum_partition, 

    }

    return hpx::when_all_n(sums_partitions.data(), sums_partitions.size())
        .then(
            [](auto&& sums_partitions)
            {
                // Sum all partition sums and return the result
                auto partitions = sums_partitions.get();

                Value result = 0;

                for(auto const& partition_id: partitions) {
                    auto partition = typename Sums::Partition{partition_id.get()};
                    auto data = partition.data().get();
                    assert(data.size() == 1);

                    result += data[0];
                }

                return result;
            }
        );












    // Sums sums{sums_partitions};



    // PartitionSums partition_sums{partitions};

    // assert(array.nr_partitions() == partition_sums.nr_partitions());
    // assert(partition_sums.nr_partitions() == partition_sums.nr_elements());












    // Store these results locally. This doesn't scale to large numbers
    // of partitions, so revisit when this becomes a problem.


    // TODO Deduce type from array instead of using a lue type directly
    // using ValueType = typename PartitionedArray::ValueType;
    // std::size_t const rank = 0;
    // using Data = ArrayPartitionData<ValueType, rank>;



    // TODO Hier verder
    // - Create a function that accepts the partition data instances of
    //   the array partitions. Call this function asynchronously as
    //   an action, on the locality of the input array partition.
    // - Configure distribution policy to create more partitions than
    //   there are localities
    // - Make the creation of components optional when instantiating a
    //   partitioned array? We want to create and assign components
    //   ourselves here.


    // Attach a continuation to each array partition passed in

    // for(std::size_t i = 0; i < array.nr_partitions(); ++i) {

    //     partition_sums.partitions()[i] = hpx::dataflow(
    //         hpx::launch::async,
    //         [](auto ){},
    //         array.partitions()[i]);
    // }

    // for(auto& partition: array.partitions()) {

    //     partition.get();


    //     PartitionSum new_partition(partition.get_id());


    //     // partition.fill(value.get());

    //     // partition->get_id(): server instance
    //     //
    //     // ‐ Create new partition (with a single value representing the
    //     //     sum of the current partition of values) on locality
    //     //     the current partition is located on
    //     // - We need the locality of the server instance


    // }


    // auto partition_sum_it = partition_sums.begin();

    // for(auto partition_it = array.partitions().begin();
    //         partition_it != array.end(); ++partition_it, ++partition_sum_it) {

    // //     // Assign to *partition_sum_it the future to the result of
    // //     // calculating the sum of *partition_id

    // //     *partition_sum_it = (*partition_it).then(
    // //         [](
    // //             auto& partition)
    // //         {
    // //             return hpx::make_ready_future<Value>(3);
    // //         }
    // //     );

    // }





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
