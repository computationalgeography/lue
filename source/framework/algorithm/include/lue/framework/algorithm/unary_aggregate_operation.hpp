#pragma once
#include "lue/framework/algorithm/functor_traits.hpp"
#include "lue/framework/core/component/partitioned_array.hpp"


namespace lue {
namespace detail {

template<
    typename Policies,
    typename InputPartition,
    typename OutputElement,
    typename Functor>
hpx::future<OutputElement> unary_aggregate_operation_partition(
    Policies const& policies,
    InputPartition const& input_partition,
    Functor functor)
{
    using InputData = DataT<InputPartition>;

    return hpx::dataflow(
        hpx::launch::async,
        hpx::util::unwrapping(

                [input_partition, policies, functor](
                    InputData const& input_partition_data)
                {
                    AnnotateFunction annotation{"unary_aggregate_operation_partition"};

                    // Aggregate nD array partition to nD array partition
                    // containing a single value

                    HPX_UNUSED(input_partition);

                    auto const& [indp] = policies.input_no_data_policies();
                    auto const& [ondp] = policies.output_no_data_policies();

                    Count const nr_elements{lue::nr_elements(input_partition_data)};

                    // The logic below assumes that, after iterating over
                    // the elements, the index can become nr_elements + 1
                    lue_assert(nr_elements < std::numeric_limits<Count>::max());

                    // Initialize result for the case that no result
                    // can be set, and no-data is not handled by the
                    // policies
                    OutputElement result{functor()};

                    if(nr_elements == 0)
                    {
                        ondp.mark_no_data(result);
                    }
                    else
                    {
                        Index idx{};

                        for(idx = 0; idx < nr_elements; ++idx)
                        {
                            if(!indp.is_no_data(input_partition_data, idx))
                            {
                                // Initialize result with first valid value
                                result = functor(input_partition_data[idx]);

                                // Aggregate subsequent valid values
                                for(++idx; idx < nr_elements; ++idx)
                                {
                                    if(!indp.is_no_data(input_partition_data, idx))
                                    {
                                        result = functor(result, input_partition_data[idx]);
                                    }
                                }

                                lue_assert(idx == nr_elements);
                            }
                        }

                        lue_assert(idx == nr_elements || idx == nr_elements + 1);

                        if(idx == nr_elements)
                        {
                            // The inner loop was not reached. This
                            // means that no valid data was found.
                            ondp.mark_no_data(result);
                        }
                    }

                    return result;
                }

            ),
            input_partition.data()
        );
}


template<
    typename Policies,
    typename InputPartition,
    typename OutputElement,
    typename Functor>
struct UnaryAggregateOperationPartitionAction:
    hpx::actions::make_action<
            decltype(&unary_aggregate_operation_partition<
                Policies, InputPartition, OutputElement, Functor>),
            &unary_aggregate_operation_partition<
                Policies, InputPartition, OutputElement, Functor>,
            UnaryAggregateOperationPartitionAction<
                Policies, InputPartition, OutputElement, Functor>
        >
{};

}  // namespace detail


template<
    typename Policies,
    typename InputElement,
    Rank rank,
    typename Functor>
hpx::future<OutputElementT<Functor>> unary_aggregate_operation(
    hpx::id_type const locality_id,
    Policies const& policies,
    ArrayPartition<InputElement, rank> const& input_partition,
    Functor const& functor)
{
    using InputPartition = ArrayPartition<InputElement, rank>;
    using OutputElement = OutputElementT<Functor>;

    detail::UnaryAggregateOperationPartitionAction<
        Policies, InputPartition, OutputElement, Functor> action;

    return hpx::dataflow(
        hpx::launch::async,

        [locality_id, action, policies, functor](
            InputPartition const& input_partition)
        {
            AnnotateFunction annotation{"unary_aggregate_operation"};

            return action(locality_id, policies, input_partition, functor);
        },

        input_partition);
}


template<
    typename Policies,
    typename InputElement,
    Rank rank,
    typename Functor>
hpx::future<OutputElementT<Functor>> unary_aggregate_operation(
    Policies const& policies,
    PartitionedArray<InputElement, rank> const& input_array,
    Functor const& functor)
{
    using InputArray = PartitionedArray<InputElement, rank>;
    using InputPartitions = PartitionsT<InputArray>;
    using InputPartition = PartitionT<InputArray>;

    using OutputElement = OutputElementT<Functor>;

    detail::UnaryAggregateOperationPartitionAction<
        Policies, InputPartition, OutputElement, Functor> action;

    Localities<rank> const& localities{input_array.localities()};
    InputPartitions const& input_partitions{input_array.partitions()};
    Count const nr_partitions{lue::nr_partitions(input_array)};
    std::vector<hpx::future<OutputElement>> partition_results(nr_partitions);

    for(Index p = 0; p < nr_partitions; ++p)
    {
        partition_results[p] = hpx::dataflow(
            hpx::launch::async,

            [locality_id=localities[p], action, policies, functor](
                InputPartition const& input_partition)
            {
                AnnotateFunction annotation{"unary_aggregate_operation"};

                return action(locality_id, policies, input_partition, functor);
            },

            input_partitions[p]);
    }

    // The partition results are being determined on their respective
    // localities. Attach a continuation that aggregates the results
    // once the partition results are ready. This continuation runs on
    // our locality.
    return hpx::when_all(
                partition_results.begin(), partition_results.end()).then(
                    hpx::util::unwrapping(

            [policies, functor](
                auto&& partition_results)
            {
                AnnotateFunction annotation{"unary_aggregate_operation"};

                auto const& [ondp] = policies.output_no_data_policies();

                std::size_t nr_results{std::size(partition_results)};

                // Initialize result for the case that no result
                // can be set, and no-data is not handled by the
                // policies
                OutputElement result{functor()};

                if(nr_results == 0)
                {
                    ondp.mark_no_data(result);
                }
                else
                {
                    OutputElement partition_result;
                    std::size_t idx;

                    for(idx = 0; idx < nr_results; ++idx)
                    {
                        partition_result = functor.partition(partition_results[idx].get());

                        // Result is output: use OutputNoDataPolicy
                        if(!ondp.is_no_data(partition_result))
                        {
                            // Initialize result with first valid value
                            result = partition_result;

                            // Aggregate subsequent valid values
                            for(++idx; idx < nr_results; ++idx)
                            {
                                partition_result = functor.partition(partition_results[idx].get());

                                if(!ondp.is_no_data(partition_result))
                                {
                                    result = functor.partition(result, partition_result);
                                }
                            }

                            lue_assert(idx == nr_results);
                        }
                    }

                    lue_assert(idx == nr_results || idx == nr_results + 1);

                    if(idx == nr_results)
                    {
                        // The inner loop was not reached. This
                        // means that no valid data was found.
                        ondp.mark_no_data(result);
                    }
                }

                return result;
            }

        ));
}

}  // namespace lue
