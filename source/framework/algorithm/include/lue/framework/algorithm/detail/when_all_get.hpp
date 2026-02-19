#pragma once
#include "lue/framework/core/array.hpp"
#include "lue/framework/partitioned_array/array_partition_decl.hpp"


namespace lue::detail {

    template<typename Element, Rank rank>
    hpx::future<Array<Element, rank>> when_all_get(Array<hpx::future<Element>, rank>&& array_of_futures)
    {
        // Given an array of futures, wait for all futures to become ready
        // and return an array with the values pointed to by the futures.

        return hpx::when_all(array_of_futures.begin(), array_of_futures.end())
            .then(
                [shape = array_of_futures.shape()](hpx::future<std::vector<hpx::future<Element>>>&& f)
                {
                    std::vector<hpx::future<Element>> vector_of_ready_futures{f.get()};
                    Array<Element, rank> array_of_elements{{shape}};

                    std::transform(
                        vector_of_ready_futures.begin(),
                        vector_of_ready_futures.end(),
                        array_of_elements.begin(),

                        [](hpx::future<Element>& future) { return future.get(); });

                    return array_of_elements;
                });
    }


    template<typename Element, Rank rank>
    hpx::future<Array<Element, rank>> when_all_get(
        Array<hpx::shared_future<Element>, rank>&& array_of_futures)
    {
        // Given an array of futures, wait for all futures to become ready
        // and return an array with the values pointed to by the futures.

        return hpx::when_all(array_of_futures.begin(), array_of_futures.end())
            .then(
                [shape = array_of_futures.shape()](hpx::future<std::vector<hpx::shared_future<Element>>>&& f)
                {
                    std::vector<hpx::shared_future<Element>> vector_of_ready_futures{f.get()};
                    Array<Element, rank> array_of_elements{{shape}};

                    std::transform(
                        vector_of_ready_futures.begin(),
                        vector_of_ready_futures.end(),
                        array_of_elements.begin(),

                        [](hpx::shared_future<Element> const& future) { return future.get(); });

                    return array_of_elements;
                });
    }


    template<typename Element, Rank rank>
    hpx::future<ArrayPartitionData<ArrayPartition<Element, rank>, rank>> when_all_get(
        ArrayPartitionData<ArrayPartition<Element, rank>, rank>&& partitions)
    {
        // Given an array of partitions, wait for all partitions to become ready
        // and return a future to an array with ready partitions

        using InputPartition = ArrayPartition<Element, rank>;

        return hpx::when_all(partitions.begin(), partitions.end())
            .then(
                hpx::unwrapping(
                    [shape = partitions.shape()](std::vector<InputPartition>&& vector_of_ready_partitions)
                    {
                        ArrayPartitionData<InputPartition, rank> array_of_partitions{{shape}};

                        std::move(
                            vector_of_ready_partitions.begin(),
                            vector_of_ready_partitions.end(),
                            array_of_partitions.begin());

                        return array_of_partitions;
                    }

                    ));
    }

}  // namespace lue::detail
