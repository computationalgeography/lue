#pragma once
#include <hpx/include/future.hpp>


namespace lue {

template<
    typename PartitionedArray,
    typename Value
>
PartitionedArray& fill(
    PartitionedArray& array,
    hpx::shared_future<Value>& value)
{
    // Attach a continuation to each partition that fills all elements with
    // the value passed in

    for(auto& partition: array) {
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
    }

    return array;
}

}  // namespace lue
