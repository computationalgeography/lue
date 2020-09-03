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
    using InputData = DataT<InputPartition>;
    using ZonesData = DataT<ZonesPartition>;
    using Aggregator = AggregatorT<Functor>;

    lue_assert(input_partition.is_ready());
    lue_assert(zones_partition.is_ready());

    return hpx::dataflow(
        hpx::launch::async,
        hpx::util::unwrapping(

                [input_partition, zones_partition](
                    InputData const& input_partition_data,
                    ZonesData const& zones_partition_data)
                {
                    AnnotateFunction annotation{"zonal_operation_partition"};

                    HPX_UNUSED(input_partition);
                    HPX_UNUSED(zones_partition);

                    Count const nr_elements{lue::nr_elements(input_partition_data)};

                    lue_assert(lue::nr_elements(zones_partition_data) == nr_elements);

                    Aggregator result{};

                    for(Index i = 0; i < nr_elements; ++i)
                    {
                        result.add(zones_partition_data[i], input_partition_data[i]);
                    }

                    return result;
                }

            ),
        input_partition.data(),
        zones_partition.data());
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
    using Offset = OffsetT<ZonesPartition>;
    using ZonesData = DataT<ZonesPartition>;
    using OutputData = DataT<OutputPartition>;

    lue_assert(zones_partition.is_ready());

    return hpx::dataflow(
        hpx::launch::async,
        hpx::util::unwrapping(

                [zones_partition, aggregator](
                    Offset const& offset,
                    ZonesData const& zones_partition_data)
                {
                    AnnotateFunction annotation{"zonal_operation_partition"};

                    HPX_UNUSED(zones_partition);

                    OutputData output_partition_data{zones_partition_data.shape()};

                    Count const nr_elements{lue::nr_elements(zones_partition_data)};

                    for(Index i = 0; i < nr_elements; ++i)
                    {
                        output_partition_data[i] = aggregator[zones_partition_data[i]];
                    }

                    return OutputPartition{hpx::find_here(), offset, std::move(output_partition_data)};
                }

            ),
        zones_partition.offset(),
        zones_partition.data());
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
    using InputArray = PartitionedArray<InputElement, rank>;
    using InputPartitions = PartitionsT<InputArray>;
    using InputPartition = PartitionT<InputArray>;

    using ZonesArray = PartitionedArray<Zone, rank>;
    using ZonesPartitions = PartitionsT<ZonesArray>;
    using ZonesPartition = PartitionT<ZonesArray>;

    using OutputArray = PartitionedArray<OutputElementT<Functor>, rank>;
    using OutputPartitions = PartitionsT<OutputArray>;
    using OutputPartition = PartitionT<OutputArray>;

    using Aggregator = AggregatorT<Functor>;
    using Aggregators = std::vector<hpx::future<Aggregator>>;

    Count const nr_partitions{lue::nr_partitions(input_array)};
    lue_assert(lue::nr_partitions(zones_array) == nr_partitions);

    Localities<rank> const& localities{input_array.localities()};
    ZonesPartitions const& zones_partitions{zones_array.partitions()};

    // -------------------------------------------------------------------------
    // 1. Per input partition, calculate a statistic per zone. This
    //     results in some operation-specific object that contains this
    //     information. Example: sum per zone
    Aggregators aggregators{static_cast<std::size_t>(nr_partitions)};

    {
        detail::ZonalOperationPartitionAction1<InputPartition, ZonesPartition, Functor> action;

        InputPartitions const& input_partitions{input_array.partitions()};

        for(Index p = 0; p < nr_partitions; ++p)
        {
            aggregators[p] = hpx::dataflow(
                hpx::launch::async,

                [locality_id=localities[p], action, functor](
                    InputPartition const& input_partition,
                    ZonesPartition const& zones_partition)
                {
                    AnnotateFunction annotation{"zonal_operation"};

                    return action(locality_id, input_partition, zones_partition, functor);
                },

                input_partitions[p],
                zones_partitions[p]);
        }
    }

    // -------------------------------------------------------------------------
    // 2. Merge the zonal statistics of all input partitions. This
    //     results in the same operation-specific object, but now
    //     containing information for the whole array.

    hpx::shared_future<Aggregator> aggregator = hpx::when_all(std::move(aggregators)).then(
        [](
            hpx::future<Aggregators>&& aggregators)
        {
            AnnotateFunction annotation{"zonal_operation"};

            Aggregator result{};

            for(hpx::future<Aggregator>& aggregator: aggregators.get())
            {
                result.merge(aggregator.get());
            }

            return result;
        });

    // -------------------------------------------------------------------------
    // 3. Per input partition, translate input zone to output statistic.

    OutputPartitions output_partitions{shape_in_partitions(input_array)};

    {
        detail::ZonalOperationPartitionAction2<ZonesPartition, OutputPartition, Functor> action;

        for(Index p = 0; p < nr_partitions; ++p)
        {
            output_partitions[p] = hpx::dataflow(
                hpx::launch::async,

                [locality_id=localities[p], action, zones_partition=zones_partitions[p]](
                    hpx::shared_future<Aggregator>const& aggregator)
                {
                    AnnotateFunction annotation{"zonal_operation"};

                    return action(locality_id, zones_partition, aggregator.get());
                },

                aggregator);
        }
    }

    return OutputArray{shape(input_array), localities, std::move(output_partitions)};
}

}  // namespace lue
