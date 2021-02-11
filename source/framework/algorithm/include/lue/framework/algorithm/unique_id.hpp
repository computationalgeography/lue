#pragma once
#include "lue/framework/core/annotate.hpp"
#include "lue/framework/core/component/partitioned_array.hpp"
#include <numeric>


namespace lue {
namespace detail {

template<
    typename InputPartition>
[[nodiscard]] hpx::future<void> unique_id_partition(
    InputPartition input_partition,  // Can't call .then on a const&
    ElementT<InputPartition> const start_value)
{
    // - Create a new collection of data elements
    // - Assign unique IDs to the cells
    // - Assign the collection of data elements
    // - Return a future that becomes ready once the data has been
    //     assigned to the existing partition

    using Data = DataT<InputPartition>;
    using Shape = ShapeT<InputPartition>;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
    auto task =
        [start_value](
            InputPartition input_partition)
        {
            // Given the partition's shape, we can create a new
            // collection for partition elements
            return input_partition.shape().then(
                hpx::util::unwrapping(

                        [input_partition, start_value](
                            Shape const& shape) mutable
                        {
                            AnnotateFunction annotation{"unique_id_partition"};

                            Data data{shape};
                            std::iota(data.begin(), data.end(), start_value);

                            return input_partition.set_data(std::move(data));
                        }

                    )
                );
        };
#pragma GCC diagnostic pop

    // Given the logic below, input_partition should be already ready. In
    // the general case (we might be called from some other algorithm)
    // it is not.
    return input_partition.is_ready()
        ? task(input_partition)
        : input_partition.then(task)
        ;
}


template<
    typename Element,
    Rank rank>
struct UniqueIDPartitionAction
{
};

}  // namespace detail


template<
    typename Partition>
struct UniqueIDPartitionAction:
    hpx::actions::make_action<
        decltype(&detail::unique_id_partition<Partition>),
        &detail::unique_id_partition<Partition>,
        UniqueIDPartitionAction<Partition>>
{};


/*!
    @brief      Fill a partitioned array in-place with unique IDs
    @tparam     Element Type of elements in the array
    @tparam     rank Rank of the array
    @param      input_array Partitioned array
    @return     Future that becomes ready once the algorithm has finished

    The existing @a input_array passed in is updated. Use the returned future if
    you need to know when the filling is done.
*/
template<
    typename Element,
    Rank rank>
[[nodiscard]] hpx::future<void> unique_id(
    PartitionedArray<Element, rank>& input_array)
{
    // - Iterate over all partitions
    //     - Determine first value for partition
    //     - Call action that fills the partition with unique IDs
    // - Return future that becomes ready once all partitions are done
    //     assigning values to all cells

    using InputArray = PartitionedArray<Element, rank>;
    using InputPartitions = PartitionsT<InputArray>;
    using InputPartition = PartitionT<InputArray>;

    InputPartitions const& input_partitions{input_array.partitions()};

    Count const nr_partitions{input_partitions.nr_elements()};

    std::vector<hpx::future<Count>> partition_sizes(nr_partitions);
    {
        // Request the sizes of all partitions and wait until they are
        // available. We need this to be able to offset each partition's
        // ID with the number of previous elements (see start_value below).
        for(Index p = 0; p < nr_partitions; ++p)
        {
            partition_sizes[p] = hpx::dataflow(
                hpx::launch::async,

                [](InputPartition const& partition)
                {
                    AnnotateFunction annotation{"unique_id"};

                    return partition.nr_elements();
                },

                input_partitions[p]);
        }
    }

    UniqueIDPartitionAction<InputPartition> action{};

    return hpx::when_all(partition_sizes).then(hpx::util::unwrapping(

            [localities=input_array.localities(), input_partitions, action](
                std::vector<hpx::future<Count>>&& partition_sizes)
            {
                AnnotateFunction annotation{"unique_id"};

                Element start_value = 0;
                Count const nr_partitions{input_partitions.nr_elements()};
                std::vector<hpx::future<void>> unique_id_partitions(nr_partitions);

                for(Index p = 0; p < nr_partitions; ++p)
                {
                    lue_assert(input_partitions[p].is_ready());

                    unique_id_partitions[p] = hpx::async(
                        action, localities[p], input_partitions[p], start_value);
                    start_value += partition_sizes[p].get();
                }

                return hpx::when_all(unique_id_partitions);
            }

        ));
}

}  // namespace lue
