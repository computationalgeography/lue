#pragma once
#include <hpx/include/future.hpp>
#include <hpx/lcos/when_all.hpp>


namespace lue {

/*!
    @brief      Fill @a array in-place with @a value
    @param      array Array to fill
    @param      value Value to assign to each cell in @a array
    @return     Future that becomes ready once the algorithm has finished

    The existing @a array passed in is updated. Use the returned future if
    you need to know when the filling is done.
*/
template<
    typename PartitionedArray,
    typename Value
>
hpx::future<void> fill(
    PartitionedArray& array,
    hpx::shared_future<Value>& value)
{
    // Attach a continuation to each partition that fills all elements with
    // the value passed in

    std::vector<hpx::future<void>> results;
    results.reserve(array.nr_partitions());

    // TODO Parallelize this also. There is no need to serialize iteration
    //      over the partitions.
    // for(typename PartitionedArray::PartitionClient& partition: array.partitions()) {
    for(auto& partition: array.partitions()) {

        partition.get();
        partition.fill(value.get());

        // results.push_back(
        //     hpx::when_all(/* hpx::launch::async, */ partition, value).then(
        //         [](
        //             auto&& partitionn,
        //             auto const& value)
        //         {
        //             // printf(partitionn);
        //             // partitionn = 5.9;
        //             // partitionn.fill(value.get());
        //         }
        //     )
        // );

        // results.push_back(hpx::dataflow(
        //     hpx::launch::async,
        //     hpx::util::unwrapping(
        //         [](
        //             auto const& partition,
        //             auto const& value)
        //         {
        //             partition.fill(value);

        //             // auto& server = *partition.component();
        //             // auto& data = server.data();

        //             // // Update partition passed in
        //             // std::fill(data.begin(), data.end(), value);
        //         }
        //     ),
        //     partition, value)
        // );

    }

    return hpx::when_all(/* hpx::launch::async, */ results);
}

}  // namespace lue
