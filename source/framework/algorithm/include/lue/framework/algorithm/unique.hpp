#pragma once
#include "lue/framework/core/component/partitioned_array.hpp"
#include <hpx/lcos/when_all.hpp>


namespace lue {
namespace detail {

template<
    typename InputPartition>
PartitionT<InputPartition, ElementT<InputPartition>, 1> unique_partition(
    InputPartition input_partition)  // Can't .then on a const& instance...
{
    using InputData = DataT<InputPartition>;
    using Element = ElementT<InputPartition>;

    using OutputPartition = PartitionT<InputPartition, Element, 1>;
    using OutputOffset = OffsetT<OutputPartition>;
    using OutputShape = ShapeT<OutputPartition>;
    using OutputData = DataT<OutputPartition>;

    lue_assert(input_partition.locality_id().get() == hpx::find_here());

    return input_partition.then(
            [](
                InputPartition const& input_partition)
            {
                auto const input_partition_server_ptr{
                    hpx::get_ptr(input_partition).get()};
                auto const& input_partition_server{*input_partition_server_ptr};

                InputData const input_partition_data{
                    input_partition_server.data()};

                // Copy values from input array into a set
                std::set<Element> unique_values(
                    input_partition_data.begin(), input_partition_data.end());
                Count const nr_unique_values =
                    static_cast<Count>(unique_values.size());
                lue_assert(
                    nr_unique_values <= input_partition_data.nr_elements());

                // Copy unique values from set into output array
                OutputData output_data{OutputShape{{nr_unique_values}}};
                lue_assert(output_data.nr_elements() == nr_unique_values);
                std::copy(
                    unique_values.begin(), unique_values.end(),
                    output_data.begin());

                return OutputPartition{
                    hpx::find_here(), OutputOffset{0}, std::move(output_data)};
            }

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


/*!
    @brief      Return the unique values in a partitioned array
    @tparam     Element Type of elements in the input array
    @tparam     rank Rank of the input array
    @tparam     Array Class template of the type of the array
    @param      input_array Partitioned array
    @return     Future to collection with unique values that becomes
                ready once the algorithm has finished

    The returned collection is a one dimensional partitioned array with
    a single partition located on the current locality.
*/
template<
    typename Element,
    Rank rank>
hpx::future<PartitionedArray<Element, 1>> unique(
    PartitionedArray<Element, rank> const& input_array)
{

    // - Determine unique values per partition
    // - Collect these values and determine overall collection of
    //     unique values
    // - The result is a 1-dimensional array with a single partition,
    //     located on the current locality

    using InputArray = PartitionedArray<Element, rank>;
    using InputPartition = PartitionT<InputArray>;

    using OutputArray = PartitionedArray<Element, 1>;
    using OutputPartitions = PartitionsT<OutputArray>;
    using OutputPartition = PartitionT<OutputArray>;
    using OutputOffset = OffsetT<OutputArray>;
    using OutputShape = ShapeT<OutputArray>;
    using OutputData = DataT<OutputPartition>;

    OutputPartitions output_partitions{OutputShape{{nr_partitions(input_array)}}};
    UniquePartitionAction<InputPartition> action;

    for(Index p = 0; p < nr_partitions(input_array); ++p)
    {
        InputPartition const& input_partition = input_array.partitions()[p];

        output_partitions[p] = hpx::dataflow(
            hpx::launch::async,
            hpx::util::unwrapping(

                    [action, input_partition](
                        hpx::id_type const locality_id)
                    {
                        return action(locality_id, input_partition);
                    }

                ),
            input_partition.locality_id());
    }

    // Collect all unique values into a single array, on the current locality

    // The unique values in each partition are being determined on
    // their respective localities. Attach a continuation that determines
    // the unique values in the whole array once the partitions are
    // ready. This continuation runs on our own locality. This implies
    // data is transported from remote localities to us.
    return hpx::when_all(
        output_partitions.begin(), output_partitions.end()).then(
                hpx::util::unwrapping(

            [](auto const& partitions) {

                // Collection of unique values of all partition results
                std::set<Element> unique_values{};

                for(auto const& partition: partitions)
                {
                    auto const data = partition.data().get();

                    unique_values.insert(data.begin(), data.end());
                }

                // Shape in elements of resulting partitioned array
                OutputShape const shape{
                    {static_cast<Count>(unique_values.size())}};

                // Shape in partitions of resulting partitioned array
                OutputShape const shape_in_partitions{{1}};

                // Copy unique values into an array-data collection
                OutputData result_values{shape};
                std::copy(
                    unique_values.begin(), unique_values.end(),
                    result_values.begin());

                // Store array data in partition component
                OutputPartition result_partition{
                    hpx::find_here(), OutputOffset{0}, std::move(result_values)};

                // Store partition component in a collection
                OutputPartitions result_partitions{
                    shape_in_partitions, std::move(result_partition)};

                // Store collection of partitions in a partitioned array
                OutputArray result_array{shape, std::move(result_partitions)};

                return result_array;
            }

        ));
}

}  // namespace lue
