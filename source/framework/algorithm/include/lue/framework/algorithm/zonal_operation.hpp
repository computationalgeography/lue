#pragma once
#include "lue/framework/algorithm/functor_traits.hpp"
#include "lue/framework/core/component/partitioned_array.hpp"


namespace lue {
namespace detail {

/*!
    @brief      For this partition, calculate some statistic per zone
    @param      input_partition Input elements
    @param      zones_partition Input zones
    @return     Future to collection of statistic per zone
*/
template<
    typename InputPartition,
    typename ZonesPartition,
    typename Functor>
hpx::future<AggregatorT<Functor>> zonal_operation_partition1(
    InputPartition const& input_partition,
    ZonesPartition const& zones_partition,
    Functor /* functor */)
{
    lue_assert(input_partition.locality_id().get() == hpx::find_here());
    lue_assert(zones_partition.locality_id().get() == hpx::find_here());

    using InputData = DataT<InputPartition>;
    using ZonesData = DataT<ZonesPartition>;
    using Aggregator = AggregatorT<Functor>;

    return hpx::dataflow(
        hpx::launch::async,

        [](
            InputPartition const& input_partition,
            ZonesPartition const& zones_partition)
        {
            auto const input_partition_server_ptr{
                hpx::get_ptr(input_partition).get()};
            auto const& input_partition_server{*input_partition_server_ptr};
            InputData input_partition_data{input_partition_server.data()};

            auto const zones_partition_server_ptr{
                hpx::get_ptr(zones_partition).get()};
            auto const& zones_partition_server{*zones_partition_server_ptr};
            ZonesData zones_partition_data{zones_partition_server.data()};

            Count const nr_elements{lue::nr_elements(input_partition_data)};

            lue_assert(lue::nr_elements(zones_partition_data) == nr_elements);

            Aggregator result{};

            for(Index i = 0; i < nr_elements; ++i)
            {
                result.add(zones_partition_data[i], input_partition_data[i]);
            }

            return result;
        },

        input_partition,
        zones_partition);
}


/*!
    @brief      For a partition, translate input zones to result values,
                given a collection of statistic per zone passed in
    @param      zones_partition Input zones
    @param      aggregator Collection of statistic per zone
    @return     Partition with per zone the corresponding statistic
*/
template<
    typename ZonesPartition,
    typename OutputPartition,
    typename Functor>
OutputPartition zonal_operation_partition2(
    ZonesPartition const& zones_partition,
    AggregatorT<Functor> const& aggregator)
{
    lue_assert(zones_partition.locality_id().get() == hpx::find_here());

    using ZonesData = DataT<ZonesPartition>;

    using OutputData = DataT<OutputPartition>;

    lue_assert(zones_partition.is_ready());

    auto const zones_partition_server_ptr{
        hpx::get_ptr(zones_partition).get()};
    auto const& zones_partition_server{*zones_partition_server_ptr};

    auto offset{zones_partition_server.offset()};
    ZonesData zones_partition_data{zones_partition_server.data()};

    OutputData output_partition_data{zones_partition_data.shape()};

    Count const nr_elements{lue::nr_elements(zones_partition_data)};

    for(Index i = 0; i < nr_elements; ++i)
    {
        output_partition_data[i] = aggregator[zones_partition_data[i]];
    }

    return OutputPartition{
        hpx::find_here(), offset, std::move(output_partition_data)};
}


template<
    typename InputPartition,
    typename ZonesPartition,
    typename Functor>
struct ZonalOperationPartitionAction1:
    hpx::actions::make_action<
            decltype(&zonal_operation_partition1<
                InputPartition, ZonesPartition, Functor>),
            &zonal_operation_partition1<
                InputPartition, ZonesPartition, Functor>,
            ZonalOperationPartitionAction1<
                InputPartition, ZonesPartition, Functor>
        >
{};


template<
    typename ZonesPartition,
    typename OutputPartition,
    typename Functor>
struct ZonalOperationPartitionAction2:
    hpx::actions::make_action<
            decltype(&zonal_operation_partition2<
                ZonesPartition, OutputPartition, Functor>),
            &zonal_operation_partition2<
                ZonesPartition, OutputPartition, Functor>,
            ZonalOperationPartitionAction2<
                ZonesPartition, OutputPartition, Functor>
        >
{};

}  // namespace detail


template<
    typename InputElement,
    typename Zone,
    Rank rank,
    typename Functor>
PartitionedArray<OutputElementT<Functor>, rank> zonal_operation(
    PartitionedArray<InputElement, rank> const& input_array,
    PartitionedArray<Zone, rank> const& zones_array,
    Functor const& functor)
{
    // 1. Per input partition, calculate a statistic per zone. This
    //     results in some operation-specific object that contains this
    //     information. Example: sum per zone
    // 2. Merge the zonal statistics of all input partitions. This
    //     results in the same operation-specific object, but now
    //     containing information for the whole array.
    // 3. Per input partition, translate input zone to output statistic.

    using InputArray = PartitionedArray<InputElement, rank>;
    using InputPartition = PartitionT<InputArray>;

    using ZonesArray = PartitionedArray<Zone, rank>;
    using ZonesPartition = PartitionT<ZonesArray>;

    using OutputArray = PartitionedArray<OutputElementT<Functor>, rank>;
    using OutputPartitions = PartitionsT<OutputArray>;
    using OutputPartition = PartitionT<OutputArray>;

    using Aggregator = AggregatorT<Functor>;
    using Aggregators = std::vector<hpx::future<Aggregator>>;

    Count const nr_partitions{lue::nr_partitions(input_array)};
    lue_assert(lue::nr_partitions(zones_array) == nr_partitions);

    // 1. ----------------------------------------------------------------------
    Aggregators aggregators{static_cast<std::size_t>(nr_partitions)};

    {
        detail::ZonalOperationPartitionAction1<
            InputPartition, ZonesPartition, Functor> action;

        for(Index p = 0; p < nr_partitions; ++p)
        {
            InputPartition const& input_partition{input_array.partitions()[p]};
            ZonesPartition const& zones_partition{zones_array.partitions()[p]};

            aggregators[p] = hpx::dataflow(
                hpx::launch::async,
                hpx::util::unwrapping(

                        [action, functor, input_partition, zones_partition](
                            hpx::id_type const locality_id)
                        {
                            return action(
                                locality_id, input_partition,
                                zones_partition, functor);
                        }

                    ),
                input_partition.locality_id());
        }
    }

    // 2. ----------------------------------------------------------------------
    hpx::shared_future<Aggregator> aggregator =
            hpx::when_all(std::move(aggregators)).then(
        [](
            hpx::future<Aggregators>&& aggregators)
        {
            Aggregator result{};

            for(hpx::future<Aggregator>& aggregator: aggregators.get())
            {
                result.merge(aggregator.get());
            }

            return result;
        });

    // 3. ----------------------------------------------------------------------
    OutputPartitions output_partitions{shape_in_partitions(input_array)};

    {
        detail::ZonalOperationPartitionAction2<
            ZonesPartition, OutputPartition, Functor> action;

        for(Index p = 0; p < nr_partitions; ++p)
        {
            ZonesPartition const& zones_partition{zones_array.partitions()[p]};

            output_partitions[p] = hpx::dataflow(
                hpx::launch::async,
                hpx::util::unwrapping(

                        [action, zones_partition](
                            hpx::id_type const locality_id,
                            Aggregator const& aggregator)
                        {
                            return action(
                                locality_id, zones_partition, aggregator);
                        }

                    ),
                zones_partition.locality_id(),
                aggregator);
        }
    }

    return OutputArray{shape(input_array), std::move(output_partitions)};
}

}  // namespace lue
