#pragma once
#include "lue/framework/algorithm/functor_traits.hpp"
#include "lue/framework/core/component/partitioned_array.hpp"


namespace lue {
namespace detail {

template<
    typename InputPartition,
    typename OutputElement,
    typename Functor>
hpx::future<OutputElement> unary_aggregate_operation_partition(
    InputPartition const& input_partition,
    Functor functor)
{
    using InputData = DataT<InputPartition>;
    using Shape = ShapeT<InputPartition>;

    return hpx::dataflow(
        hpx::launch::async,

        [functor](
            InputPartition const& input_partition)
        {
            // Aggregate nD array partition to nD array partition
            // containing a single value
            Shape shape;
            std::fill(shape.begin(), shape.end(), 1);

            auto const input_partition_server_ptr{
                hpx::get_ptr(input_partition).get()};
            auto const& input_partition_server{*input_partition_server_ptr};

            InputData input_partition_data{input_partition_server.data()};

            // Initialize result for the case that the partition is empty
            OutputElement result{functor()};

            if(!input_partition_data.empty())
            {
                // Initialize result with first value
                result = functor(input_partition_data[0]);

                // Aggregate subsequent values
                for(Index idx = 1; idx < input_partition_data.nr_elements();
                        ++idx)
                {
                    result = functor(result, input_partition_data[idx]);
                }
            }

            return result;
        },

        input_partition);
}


template<
    typename InputPartition,
    typename OutputElement,
    typename Functor>
struct UnaryAggregateOperationPartitionAction:
    hpx::actions::make_action<
            decltype(&unary_aggregate_operation_partition<
                InputPartition, OutputElement, Functor>),
            &unary_aggregate_operation_partition<
                InputPartition, OutputElement, Functor>,
            UnaryAggregateOperationPartitionAction<
                InputPartition, OutputElement, Functor>
        >
{};

}  // namespace detail


template<
    typename InputElement,
    Rank rank,
    typename Functor>
hpx::future<OutputElementT<Functor>> unary_aggregate_operation(
    hpx::id_type const locality_id,
    ArrayPartition<InputElement, rank> const& input_partition,
    Functor const& functor)
{
    using InputPartition = ArrayPartition<InputElement, rank>;
    using OutputElement = OutputElementT<Functor>;

    detail::UnaryAggregateOperationPartitionAction<InputPartition, OutputElement, Functor> action;

    return hpx::dataflow(
        hpx::launch::async,

        [locality_id, action, functor](
            InputPartition const& input_partition)
        {
            return action(locality_id, input_partition, functor);
        },

        input_partition);
}


template<
    typename InputElement,
    Rank rank,
    typename Functor>
hpx::future<OutputElementT<Functor>> unary_aggregate_operation(
    PartitionedArray<InputElement, rank> const& input_array,
    Functor const& functor)
{
    using InputArray = PartitionedArray<InputElement, rank>;
    using InputPartitions = PartitionsT<InputArray>;
    using InputPartition = PartitionT<InputArray>;

    using OutputElement = OutputElementT<Functor>;

    detail::UnaryAggregateOperationPartitionAction<
        InputPartition, OutputElement, Functor> action;

    Localities<rank> const& localities{input_array.localities()};
    InputPartitions const& input_partitions{input_array.partitions()};
    Count const nr_partitions{lue::nr_partitions(input_array)};
    std::vector<hpx::future<OutputElement>> partition_results(nr_partitions);

    for(Index p = 0; p < nr_partitions; ++p)
    {
        partition_results[p] = hpx::dataflow(
            hpx::launch::async,

            [locality_id=localities[p], action, functor](
                InputPartition const& input_partition)
            {
                return action(locality_id, input_partition, functor);
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

            [functor](
                auto&& partition_results)
            {
                // Initialize result for the case the array is empty
                OutputElement result{functor()};

                if(!partition_results.empty())
                {
                    // Initialize result
                    result = functor.partition(partition_results[0].get());

                    // Aggregate
                    for(std::size_t idx = 1; idx < partition_results.size(); ++idx)
                    {
                        result = functor.partition(result, partition_results[idx].get());
                    }
                }

                return result;
            }

        ));
}

}  // namespace lue
