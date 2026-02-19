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
                        @param      input_element Input element to aggregate
                        @param      zones_partition Input zones
                        @return     Future to collection of statistic per zone
                    */
                    static hpx::future<AggregatorT<Functor>> zonal_operation_partition(
                        Policies const& policies,
                        InputElement const input_element,
                        ZonesPartition const& zones_partition,
                        Functor /* functor */)
                    {
                        using ZonesData = DataT<ZonesPartition>;
                        using Aggregator = AggregatorT<Functor>;

                        lue_hpx_assert(zones_partition.is_ready());

                        return hpx::dataflow(
                            hpx::launch::async,
                            hpx::unwrapping(

                                [policies, input_element, zones_partition](
                                    ZonesData const& zones_partition_data)
                                {
                                    AnnotateFunction const annotation{
                                        std::format("{}: partition", functor_name<Functor>)};

                                    HPX_UNUSED(zones_partition);

                                    auto const& dp = policies.domain_policy();
                                    auto const& indp1 =
                                        std::get<0>(policies.inputs_policies()).input_no_data_policy();
                                    auto const& indp2 =
                                        std::get<1>(policies.inputs_policies()).input_no_data_policy();

                                    Aggregator result{};

                                    if (!indp1.is_no_data(input_element))
                                    {
                                        Count const nr_elements{lue::nr_elements(zones_partition_data)};

                                        for (Index i = 0; i < nr_elements; ++i)
                                        {
                                            if (!indp2.is_no_data(zones_partition_data, i))
                                            {
                                                if (dp.within_domain(zones_partition_data[i], input_element))
                                                {
                                                    result.add(zones_partition_data[i], input_element);
                                                }
                                            }
                                        }
                                    }

                                    return result;
                                }

                                ),
                            zones_partition.data());
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
                    static hpx::future<AggregatorT<Functor>> zonal_operation_partition(
                        Policies const& policies,
                        InputPartition const& input_partition,
                        ZonesPartition const& zones_partition,
                        Functor /* functor */)
                    {
                        using InputData = DataT<InputPartition>;
                        using ZonesData = DataT<ZonesPartition>;
                        using Aggregator = AggregatorT<Functor>;

                        lue_hpx_assert(input_partition.is_ready());
                        lue_hpx_assert(zones_partition.is_ready());

                        return hpx::dataflow(
                            hpx::launch::async,
                            hpx::unwrapping(

                                [policies, input_partition, zones_partition](
                                    InputData const& input_partition_data,
                                    ZonesData const& zones_partition_data)
                                {
                                    AnnotateFunction const annotation{
                                        std::format("{}: partition", functor_name<Functor>)};

                                    HPX_UNUSED(input_partition);
                                    HPX_UNUSED(zones_partition);

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
                                }

                                ),
                            input_partition.data(),
                            zones_partition.data());
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
            Policies const& policies,
            ZonesPartition const& zones_partition,
            AggregatorT<Functor> const& aggregator) -> OutputPartition
        {
            using Offset = OffsetT<ZonesPartition>;
            using ZonesData = DataT<ZonesPartition>;
            using OutputData = DataT<OutputPartition>;

            lue_hpx_assert(zones_partition.is_ready());

            return hpx::dataflow(
                hpx::launch::async,
                hpx::unwrapping(

                    [policies, zones_partition, aggregator](
                        Offset const& offset, ZonesData const& zones_partition_data)
                    {
                        AnnotateFunction const annotation{
                            std::format("{}: partition: reclass", functor_name<Functor>)};

                        HPX_UNUSED(zones_partition);

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
                    }

                    ),
                zones_partition.offset(),
                zones_partition.data());
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
    PartitionedArray<OutputElementT<Functor>, rank> zonal_operation(
        Policies const& policies,
        hpx::shared_future<InputElement> const input_element,
        PartitionedArray<Zone, rank> const& zones_array,
        Functor const& functor)
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

            for (Index p = 0; p < nr_partitions; ++p)
            {
                aggregators[p] = hpx::dataflow(
                    hpx::launch::async,

                    [locality_id = localities[p], action, policies, functor](
                        hpx::shared_future<InputElement> const& input_element,
                        ZonesPartition const& zones_partition)
                    { return action(locality_id, policies, input_element.get(), zones_partition, functor); },

                    input_element,
                    zones_partitions[p]);
            }
        }

        // -------------------------------------------------------------------------
        // 2. Merge the zonal statistics of all partitions. This
        //     results in the same operation-specific object, but now
        //     containing information for the whole array.

        hpx::shared_future<Aggregator> aggregator =
            hpx::when_all(std::move(aggregators))
                .then(
                    [](hpx::future<Aggregators>&& aggregators)
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

            for (Index p = 0; p < nr_partitions; ++p)
            {
                output_partitions[p] = hpx::dataflow(
                    hpx::launch::async,

                    [locality_id = localities[p], action, policies, zones_partition = zones_partitions[p]](
                        hpx::shared_future<Aggregator> const& aggregator)
                    { return action(locality_id, policies, zones_partition, aggregator.get()); },

                    aggregator);
            }
        }

        return OutputArray{shape(zones_array), std::move(localities), std::move(output_partitions)};
    }


    template<typename Policies, typename InputElement, typename Zone, Rank rank, typename Functor>
    PartitionedArray<OutputElementT<Functor>, rank> zonal_operation(
        Policies const& policies,
        InputElement const input_element,
        PartitionedArray<Zone, rank> const& zones_array,
        Functor const& functor)
    {
        return zonal_operation(
            policies, hpx::make_ready_future<InputElement>(input_element).share(), zones_array, functor);
    }


    template<typename Policies, typename InputElement, typename Zone, Rank rank, typename Functor>
    PartitionedArray<OutputElementT<Functor>, rank> zonal_operation(
        Policies const& policies,
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

            for (Index p = 0; p < nr_partitions; ++p)
            {
                aggregators[p] = hpx::dataflow(
                    hpx::launch::async,

                    [locality_id = localities[p], action, policies, functor](
                        InputPartition const& input_partition, ZonesPartition const& zones_partition)
                    {
                        AnnotateFunction const annotation{
                            std::format("{}: partition: call statistic action", functor_name<Functor>)};

                        return action(locality_id, policies, input_partition, zones_partition, functor);
                    },

                    input_partitions[p],
                    zones_partitions[p]);
            }
        }

        // -------------------------------------------------------------------------
        // 2. Merge the zonal statistics of all input partitions. This
        //     results in the same operation-specific object, but now
        //     containing information for the whole array.

        hpx::shared_future<Aggregator> aggregator =
            hpx::when_all(std::move(aggregators))
                .then(
                    [](hpx::future<Aggregators>&& aggregators)
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

            for (Index p = 0; p < nr_partitions; ++p)
            {
                output_partitions[p] = hpx::dataflow(
                    hpx::launch::async,

                    [locality_id = localities[p], action, policies, zones_partition = zones_partitions[p]](
                        hpx::shared_future<Aggregator> const& aggregator)
                    {
                        AnnotateFunction const annotation{
                            std::format("{}: partition: call reclass action", functor_name<Functor>)};

                        return action(locality_id, policies, zones_partition, aggregator.get());
                    },

                    aggregator);
            }
        }

        return OutputArray{shape(zones_array), std::move(localities), std::move(output_partitions)};
    }

}  // namespace lue
