#include "lue/framework/core/type_traits.hpp"
#include <hpx/include/lcos.hpp>


namespace lue {

// If this is needed elsewhere, put it in algorithm/functor_traits.hpp
// Refactor with focal_operation.hpp
template<
    typename Functor>
using OutputElementT = typename Functor::OutputElement;


namespace detail {

template<
    typename InputPartition,
    typename OutputPartition,
    typename Functor>
OutputPartition unary_local_operation_partition(
    InputPartition const& input_partition,
    Functor const& functor)
{
    assert(
        hpx::get_colocation_id(input_partition.get_id()).get() ==
        hpx::find_here());

    using InputData = DataT<InputPartition>;

    using OutputData = DataT<OutputPartition>;

    return hpx::dataflow(
        hpx::launch::async,
        hpx::util::annotated_function(
            hpx::util::unwrapping(

                [functor](
                    InputData&& input_partition_data)
                {
                    OutputData output_partition_data{
                        input_partition_data.shape()};

                    std::transform(
                        input_partition_data.begin(),
                        input_partition_data.end(),
                        output_partition_data.begin(),
                        functor);

                    return OutputPartition{
                        hpx::find_here(),
                        std::move(output_partition_data)};
                }

            ),
            "unary_local_operation_partition"),

        input_partition.data(CopyMode::share));
}


template<
    typename InputPartition,
    typename OutputPartition,
    typename Functor>
struct UnaryLocalOperationPartitionAction:
    hpx::actions::make_action<
        decltype(&unary_local_operation_partition<InputPartition, OutputPartition, Functor>),
        &unary_local_operation_partition<InputPartition, OutputPartition, Functor>,
        UnaryLocalOperationPartitionAction<InputPartition, OutputPartition, Functor>>
{};

}  // namespace detail


template<
    typename InputArray,
    typename Functor>
PartitionedArrayT<InputArray, OutputElementT<Functor>> unary_local_operation(
    InputArray const& input_array,
    Functor const& functor)
{
    using InputPartition = PartitionT<InputArray>;

    using OutputArray = PartitionedArrayT<InputArray, OutputElementT<Functor>>;
    using OutputPartitions = PartitionsT<OutputArray>;
    using OutputPartition = PartitionT<InputArray>;

    detail::UnaryLocalOperationPartitionAction<
        InputPartition, OutputPartition, Functor> action;
    OutputPartitions output_partitions{shape_in_partitions(input_array)};

    for(Index p = 0; p < nr_partitions(input_array); ++p) {

        output_partitions[p] = hpx::dataflow(
            hpx::launch::async,
            hpx::util::annotated_function(

                [action, functor](
                    InputPartition const& input_partition,
                    hpx::future<hpx::id_type>&& locality_id)
                {
                    return action(
                        locality_id.get(),
                        input_partition,
                        functor);
                },

                "spawn_unary_local_operation_partition"),

            input_array.partitions()[p],
            hpx::get_colocation_id(input_array.partitions()[p].get_id()));

    }

    return OutputArray{shape(input_array), std::move(output_partitions)};
}

}  // namespace lue
