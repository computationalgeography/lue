#pragma once
#include "lue/framework/algorithm/detail/verify_compatible.hpp"
#include "lue/framework/algorithm/functor_traits.hpp"
#include "lue/framework/core/annotate.hpp"
#include "lue/framework/partitioned_array_decl.hpp"
#include "lue/macro.hpp"


namespace lue {
    namespace detail {
        namespace zonal_operation {

            template<typename Policies, typename Input, typename ZonesPartition, typename Functor>
            class OverloadPicker
            {

                public:

                    using InputElement = Input;


                    /*!
                        @brief      For this partition, calculate some statistic per zone
                        @param      input_scalar Input element to aggregate
                        @param      zones_partition Input zones
                        @return     Future to collection of statistic per zone
                    */
                    static auto zonal_operation_partition(
                        Policies const& policies,
                        hpx::shared_future<InputElement> const& input_scalar,
                        ZonesPartition const& zones_partition,
                        Functor /* functor */) -> hpx::future<AggregatorT<Functor>>
                    {
                        using ZonesData = DataT<ZonesPartition>;
                        using Aggregator = AggregatorT<Functor>;

                        return hpx::dataflow(
                            hpx::launch::async,

                            [policies](
                                hpx::shared_future<InputElement> const& input_scalar,
                                ZonesPartition const& zones_partition) -> auto
                            {
                                AnnotateFunction const annotation{
                                    std::format("{}: partition", functor_name<Functor>)};

                                InputElement const input_value = input_scalar.get();
                                ZonesData const zones_partition_data =
                                    zones_partition.data(hpx::launch::sync);

                                auto const& dp = policies.domain_policy();
                                auto const& indp1 =
                                    std::get<0>(policies.inputs_policies()).input_no_data_policy();
                                auto const& indp2 =
                                    std::get<1>(policies.inputs_policies()).input_no_data_policy();

                                Aggregator result{};

                                if (!indp1.is_no_data(input_value))
                                {
                                    Count const nr_elements{lue::nr_elements(zones_partition_data)};

                                    for (Index i = 0; i < nr_elements; ++i)
                                    {
                                        if (!indp2.is_no_data(zones_partition_data, i))
                                        {
                                            if (dp.within_domain(zones_partition_data[i], input_value))
                                            {
                                                result.add(zones_partition_data[i], input_value);
                                            }
                                        }
                                    }
                                }

                                return result;
                            },

                            input_scalar,
                            zones_partition);
                    }


                    struct Action:
                        hpx::actions::make_action<
                            decltype(&zonal_operation_partition),
                            &zonal_operation_partition,
                            Action>::type
                    {
                    };
            };


            template<typename Policies, typename InputElement, typename ZonesPartition, typename Functor>
            class OverloadPicker<
                Policies,
                ArrayPartition<InputElement, rank<ZonesPartition>>,
                ZonesPartition,
                Functor>
            {

                public:

                    using InputPartition = ArrayPartition<InputElement, rank<ZonesPartition>>;


                    /*!
                        @brief      For this partition, calculate some statistic per zone
                        @param      input_partition Input elements to aggregate
                        @param      zones_partition Input zones
                        @return     Future to collection of statistic per zone
                    */
                    static auto zonal_operation_partition(
                        Policies const& policies,
                        InputPartition const& input_partition,
                        ZonesPartition const& zones_partition,
                        Functor /* functor */) -> hpx::future<AggregatorT<Functor>>
                    {
                        using InputData = DataT<InputPartition>;
                        using ZonesData = DataT<ZonesPartition>;
                        using Aggregator = AggregatorT<Functor>;

                        return hpx::dataflow(
                            hpx::launch::async,

                            [policies](
                                InputPartition const& input_partition,
                                ZonesPartition const& zones_partition) -> auto
                            {
                                AnnotateFunction const annotation{
                                    std::format("{}: partition", functor_name<Functor>)};

                                InputData const input_partition_data =
                                    input_partition.data(hpx::launch::sync);
                                ZonesData const zones_partition_data =
                                    zones_partition.data(hpx::launch::sync);

                                auto const& dp = policies.domain_policy();
                                auto const& indp1 =
                                    std::get<0>(policies.inputs_policies()).input_no_data_policy();
                                auto const& indp2 =
                                    std::get<1>(policies.inputs_policies()).input_no_data_policy();

                                Count const nr_elements{lue::nr_elements(zones_partition_data)};

                                lue_hpx_assert(lue::nr_elements(input_partition_data) == nr_elements);

                                Aggregator result{};

                                for (Index i = 0; i < nr_elements; ++i)
                                {
                                    if (!indp1.is_no_data(input_partition_data, i) &&
                                        !indp2.is_no_data(zones_partition_data, i))
                                    {
                                        if (dp.within_domain(
                                                zones_partition_data[i], input_partition_data[i]))
                                        {
                                            result.add(zones_partition_data[i], input_partition_data[i]);
                                        }
                                    }
                                }

                                return result;
                            },

                            input_partition,
                            zones_partition);
                    }


                    struct Action:
                        hpx::actions::make_action<
                            decltype(&zonal_operation_partition),
                            &zonal_operation_partition,
                            Action>::type
                    {
                    };
            };

        }  // Namespace zonal_operation


        /*!
            @brief      For a partition, translate input zones to result values,
                        given a collection of statistic per zone passed in
            @param      zones_partition Input zones
            @param      aggregator Collection of statistic per zone
            @return     Partition with per zone the corresponding statistic
        */
        template<typename Policies, typename ZonesPartition, typename OutputPartition, typename Functor>
        auto zonal_operation_partition2(
            Policies const& policies, ZonesPartition const& zones_partition, AggregatorT<Functor> aggregator)
            -> OutputPartition
        {
            using Offset = OffsetT<ZonesPartition>;
            using ZonesData = DataT<ZonesPartition>;
            using OutputData = DataT<OutputPartition>;

            return hpx::dataflow(
                hpx::launch::async,

                [policies,
                 aggregator = std::move(aggregator)](ZonesPartition const& zones_partition) -> OutputPartition
                {
                    AnnotateFunction const annotation{
                        std::format("{}: partition: reclass", functor_name<Functor>)};

                    ZonesData const zones_partition_data = zones_partition.data(hpx::launch::sync);
                    Offset const offset = zones_partition.offset(hpx::launch::sync);
                    OutputData output_partition_data{zones_partition_data.shape()};

                    auto const& ondp = std::get<0>(policies.outputs_policies()).output_no_data_policy();

                    Count const nr_elements{lue::nr_elements(zones_partition_data)};

                    for (Index i = 0; i < nr_elements; ++i)
                    {
                        if (!aggregator.contains(zones_partition_data[i]))
                        {
                            ondp.mark_no_data(output_partition_data, i);
                        }
                        else
                        {
                            output_partition_data[i] = aggregator[zones_partition_data[i]];
                        }
                    }

                    return OutputPartition{hpx::find_here(), offset, std::move(output_partition_data)};
                },

                zones_partition);
        }


        template<typename Policies, typename T, typename ZonesPartition, typename Functor>
        using ZonalOperationPartitionAction1 =
            typename zonal_operation::OverloadPicker<Policies, T, ZonesPartition, Functor>::Action;


        template<typename Policies, typename ZonesPartition, typename OutputPartition, typename Functor>
        struct ZonalOperationPartitionAction2:
            hpx::actions::make_action<
                decltype(&zonal_operation_partition2<Policies, ZonesPartition, OutputPartition, Functor>),
                &zonal_operation_partition2<Policies, ZonesPartition, OutputPartition, Functor>,
                ZonalOperationPartitionAction2<Policies, ZonesPartition, OutputPartition, Functor>>::type
        {
        };

    }  // namespace detail


    template<typename Policies, typename InputElement, typename Zone, Rank rank, typename Functor>
    auto zonal_operation(
        Policies const& policies,
        hpx::shared_future<InputElement> const input_scalar,
        PartitionedArray<Zone, rank> const& zones_array,
        Functor const& functor) -> PartitionedArray<OutputElementT<Functor>, rank>
    {
        using ZonesArray = PartitionedArray<Zone, rank>;
        using ZonesPartitions = PartitionsT<ZonesArray>;
        using ZonesPartition = PartitionT<ZonesArray>;

        using OutputArray = PartitionedArray<OutputElementT<Functor>, rank>;
        using OutputPartitions = PartitionsT<OutputArray>;
        using OutputPartition = PartitionT<OutputArray>;

        using Aggregator = AggregatorT<Functor>;
        using Aggregators = std::vector<hpx::future<Aggregator>>;

        Count const nr_partitions{lue::nr_partitions(zones_array)};

        Localities<rank> localities{zones_array.localities()};
        ZonesPartitions const& zones_partitions{zones_array.partitions()};

        // -------------------------------------------------------------------------
        // 1. Per partition, calculate a statistic per zone. This
        //     results in some operation-specific object that contains this
        //     information. Example: sum per zone
        Aggregators aggregators{static_cast<std::size_t>(nr_partitions)};

        {
            detail::ZonalOperationPartitionAction1<Policies, InputElement, ZonesPartition, Functor> action;

            for (Index partition_idx = 0; partition_idx < nr_partitions; ++partition_idx)
            {
                aggregators[partition_idx] = hpx::async(
                    action,
                    localities[partition_idx],
                    policies,
                    input_scalar,
                    zones_partitions[partition_idx],
                    functor);
            }
        }

        // -------------------------------------------------------------------------
        // 2. Merge the zonal statistics of all partitions. This
        //     results in the same operation-specific object, but now
        //     containing information for the whole array.

        hpx::shared_future<Aggregator> aggregator =
            hpx::when_all(std::move(aggregators))
                .then(
                    [](hpx::future<Aggregators>&& aggregators) -> auto
                    {
                        AnnotateFunction const annotation{
                            std::format("{}: aggregate", functor_name<Functor>)};

                        Aggregator result{};

                        for (hpx::future<Aggregator>& aggregator : aggregators.get())
                        {
                            result.merge(aggregator.get());
                        }

                        return result;
                    });

        // -------------------------------------------------------------------------
        // 3. Per input partition, translate input zone to output statistic.

        OutputPartitions output_partitions{shape_in_partitions(zones_array)};

        {
            detail::ZonalOperationPartitionAction2<Policies, ZonesPartition, OutputPartition, Functor> action;

            for (Index partition_idx = 0; partition_idx < nr_partitions; ++partition_idx)
            {
                output_partitions[partition_idx] = hpx::dataflow(
                    hpx::launch::async,

                    [locality_id = localities[partition_idx],
                     action,
                     policies,
                     zones_partition = zones_partitions[partition_idx]](
                        hpx::shared_future<Aggregator> const& aggregator) -> OutputPartition
                    { return hpx::async(action, locality_id, policies, zones_partition, aggregator.get()); },

                    aggregator);
            }
        }

        return OutputArray{shape(zones_array), std::move(localities), std::move(output_partitions)};
    }


    template<typename Policies, typename InputElement, typename Zone, Rank rank, typename Functor>
    auto zonal_operation(
        Policies const& policies,
        InputElement const input_value,
        PartitionedArray<Zone, rank> const& zones_array,
        Functor const& functor) -> PartitionedArray<OutputElementT<Functor>, rank>
    {
        return zonal_operation(
            policies, hpx::make_ready_future<InputElement>(input_value).share(), zones_array, functor);
    }


    template<typename Policies, typename InputElement, typename Zone, Rank rank, typename Functor>
    auto zonal_operation(
        Policies const& policies,
        PartitionedArray<InputElement, rank> const& input_array,
        PartitionedArray<Zone, rank> const& zones_array,
        Functor const& functor) -> PartitionedArray<OutputElementT<Functor>, rank>
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

        detail::verify_compatible(input_array, zones_array);

        Count const nr_partitions{lue::nr_partitions(zones_array)};

        Localities<rank> localities{zones_array.localities()};
        ZonesPartitions const& zones_partitions{zones_array.partitions()};

        // -------------------------------------------------------------------------
        // 1. Per input partition, calculate a statistic per zone. This
        //     results in some operation-specific object that contains this
        //     information. Example: sum per zone
        Aggregators aggregators{static_cast<std::size_t>(nr_partitions)};

        {
            detail::ZonalOperationPartitionAction1<Policies, InputPartition, ZonesPartition, Functor> action;

            InputPartitions const& input_partitions{input_array.partitions()};

            for (Index partition_idx = 0; partition_idx < nr_partitions; ++partition_idx)
            {
                aggregators[partition_idx] = hpx::async(
                    action,
                    localities[partition_idx],
                    policies,
                    input_partitions[partition_idx],
                    zones_partitions[partition_idx],
                    functor);
            }
        }

        // -------------------------------------------------------------------------
        // 2. Merge the zonal statistics of all input partitions. This
        //     results in the same operation-specific object, but now
        //     containing information for the whole array.

        hpx::shared_future<Aggregator> aggregator =
            hpx::when_all(std::move(aggregators))
                .then(
                    [](hpx::future<Aggregators>&& aggregators) -> auto
                    {
                        AnnotateFunction const annotation{
                            std::format("{}: array: merge", functor_name<Functor>)};

                        Aggregator result{};

                        for (hpx::future<Aggregator>& aggregator : aggregators.get())
                        {
                            result.merge(aggregator.get());
                        }

                        return result;
                    });

        // -------------------------------------------------------------------------
        // 3. Per input partition, translate input zone to output statistic.

        OutputPartitions output_partitions{shape_in_partitions(input_array)};

        {
            detail::ZonalOperationPartitionAction2<Policies, ZonesPartition, OutputPartition, Functor> action;

            for (Index partition_idx = 0; partition_idx < nr_partitions; ++partition_idx)
            {
                output_partitions[partition_idx] = hpx::dataflow(
                    hpx::launch::async,

                    [locality_id = localities[partition_idx],
                     action,
                     policies,
                     zones_partition = zones_partitions[partition_idx]](
                        hpx::shared_future<Aggregator> const& aggregator) -> OutputPartition
                    {
                        AnnotateFunction const annotation{
                            std::format("{}: partition: call reclass action", functor_name<Functor>)};

                        return hpx::async(action, locality_id, policies, zones_partition, aggregator.get());
                    },

                    aggregator);
            }
        }

        return OutputArray{shape(zones_array), std::move(localities), std::move(output_partitions)};
    }

}  // namespace lue
