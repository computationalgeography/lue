#pragma once
#include "lue/framework/core/component/array_partition.hpp"
#include <hpx/include/lcos.hpp>


namespace lue {
namespace detail {

template<
    typename Partition>
PartitionT<Partition, ElementT<Partition>, 1> unique_partition(
    Partition const& partition)
{
    using InputData = DataT<Partition>;
    using Element = ElementT<Partition>;

    using OutputPartition = PartitionT<Partition, Element, 1>;
    using OutputData = DataT<OutputPartition>;
    using OutputShape = ShapeT<OutputPartition>;

    return partition.data().then(
        hpx::util::unwrapping(
            // TODO Pass by copy?
            [partition](InputData const& partition_data)
            {
                // Copy values from input array into a set
                std::set<Element> unique_values(
                    partition_data.begin(), partition_data.end());
                assert(unique_values.size() <= partition_data.size());

                // Copy unique values from set into output array
                OutputData result{OutputShape{{unique_values.size()}}};
                assert(result.size() == unique_values.size());
                std::copy(
                    unique_values.begin(), unique_values.end(),
                    result.begin());

                return OutputPartition{partition.get_id(), result};
            }
        )
    );
}

}  // namespace detail


template<
    typename Partition>
struct UniquePartitionAction:
    hpx::actions::make_action<
        decltype(&detail::unique_partition<Partition>),
        &detail::unique_partition<Partition>,
        UniquePartitionAction<Partition>>
{};


template<
    typename Array>
hpx::future<PartitionedArrayT<Array, ElementT<Array>, 1>> unique(
    Array const& array)
{
    // - Determine unique values per partition
    // - Collect these values and determine overall collection of
    //     unique values
    // - The result is a 1-dimensional array with a single partition,
    //     located on the current locality

    using InputPartition = PartitionT<Array>;
    using Element = ElementT<Array>;

    using OutputArray = PartitionedArrayT<Array, Element, 1>;
    using OutputPartition = PartitionT<OutputArray>;
    using OutputPartitions = PartitionsT<OutputArray>;
    using OutputShape = ShapeT<OutputArray>;

    using OutputData = DataT<OutputPartition>;

    OutputPartitions unique_partitions{OutputShape{{nr_partitions(array)}}};
    UniquePartitionAction<InputPartition> unique_partition_action;

    // Attach a continuation to each array partition, that determines
    // the unique values in that partition. These continuations run
    // on the same localities as where the partitions themselves are
    // located.
    for(std::size_t p = 0; p < nr_partitions(array); ++p) {

       InputPartition const& input_partition = array.partitions()[p];

       unique_partitions[p] = hpx::dataflow(
           hpx::launch::async,
           unique_partition_action,
           // TODO Is this efficient?
           hpx::get_colocation_id(hpx::launch::sync, input_partition.get_id()),
           input_partition
       );

    }

    // Collect all unique values into a single array, on the current locality

    // The unique values in each partition are being determined on
    // their respective localities. Attach a continuation that determines
    // the unique values in the whole array once the partitions are
    // ready. This continuation runs on our own locality. This implies
    // data is transported from remote localities to us.
    return hpx::when_all(unique_partitions.begin(), unique_partitions.end())
        .then(
            [](auto&& f) {
                auto const unique_partitions = f.get();

                // Collection of unique values of all partition results
                std::set<Element> unique_values;

                for(auto const& unique_partition: unique_partitions) {

                    OutputData const& unique_partition_values =
                        unique_partition.data().get();

                    unique_values.insert(
                        unique_partition_values.begin(),
                        unique_partition_values.end());

                }

                // Shape in elements of resulting partitioned array
                OutputShape shape{{unique_values.size()}};

                // Shape in partitions of resulting partitioned array
                OutputShape shape_in_partitions{{1}};

                // Copy unique values into an array-data collection
                OutputData result_values{shape};
                std::copy(
                    unique_values.begin(), unique_values.end(),
                    result_values.begin());

                // Store array data in partition component
                OutputPartition result_partition{
                    // TODO Does this work? colocated is called on the
                    //     result of find_here. What does that mean?
                    hpx::find_here(), result_values};

                // Store partition component in a collection
                OutputPartitions result_partitions{
                    shape_in_partitions, std::move(result_partition)};

                // Store collection of partitions in a partitioned array
                OutputArray result_array{shape, std::move(result_partitions)};

                return result_array;
            }
    );
}

}  // namespace lue
