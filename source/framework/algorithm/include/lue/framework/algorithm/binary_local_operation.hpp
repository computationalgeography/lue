#pragma once
#include "lue/framework/algorithm/functor_traits.hpp"
#include "lue/framework/core/component/partitioned_array.hpp"


namespace lue {
namespace detail {
namespace binary_local_operation {

template<
    typename T1,
    typename T2,
    typename OutputPartition,
    typename Functor,
    typename Enable=void>
class OverloadPicker
{
};


template<
    typename InputPartition,
    typename OutputPartition,
    typename Functor>
class OverloadPicker<
    InputPartition,
    InputPartition,
    OutputPartition,
    Functor,
    typename std::enable_if_t<is_array_partition_v<InputPartition>>>

{

public:

    static OutputPartition binary_local_operation_partition(
        InputPartition const& input_partition1,
        InputPartition const& input_partition2,
        Functor functor)
    {
        using InputData = DataT<InputPartition>;
        using OutputData = DataT<OutputPartition>;

        lue_assert(input_partition1.locality_id().get() == hpx::find_here());
        lue_assert(input_partition2.locality_id().get() == hpx::find_here());

        return hpx::dataflow(
            hpx::launch::async,

            [functor](
                InputPartition const& input_partition1,
                InputPartition const& input_partition2)
            {
                auto const input_partition_server_ptr1{
                    hpx::get_ptr(input_partition1).get()};
                auto const& input_partition_server1{
                    *input_partition_server_ptr1};

                auto const input_partition_server_ptr2{
                    hpx::get_ptr(input_partition2).get()};
                auto const& input_partition_server2{
                    *input_partition_server_ptr2};

                auto offset{input_partition_server1.offset()};
                lue_assert(input_partition_server2.offset() == offset);

                InputData input_partition_data1{
                    input_partition_server1.data()};
                InputData input_partition_data2{
                    input_partition_server2.data()};

                OutputData output_partition_data{
                    input_partition_data1.shape()};

                std::transform(
                    input_partition_data1.begin(),
                    input_partition_data1.end(),
                    input_partition_data2.begin(),
                    output_partition_data.begin(),
                    functor);

                return OutputPartition{
                    hpx::find_here(), offset,
                    std::move(output_partition_data)};
            },

            input_partition1,
            input_partition2);
    }

    struct Action:
        hpx::actions::make_action<
            decltype(&binary_local_operation_partition),
            &binary_local_operation_partition,
            Action>
    {};

};


template<
    typename InputPartition,
    typename OutputPartition,
    typename Functor>
class OverloadPicker<
    InputPartition,
    ElementT<InputPartition>,
    OutputPartition,
    Functor>

{

public:

    using InputElement = ElementT<InputPartition>;

    static OutputPartition binary_local_operation_partition(
        InputPartition const& input_partition,
        hpx::shared_future<InputElement> const input_scalar,
        Functor functor)
    {
        using InputData = DataT<InputPartition>;
        using OutputData = DataT<OutputPartition>;

        lue_assert(input_partition.locality_id().get() == hpx::find_here());

        return hpx::dataflow(
            hpx::launch::async,

            [functor](
                InputPartition const& input_partition,
                hpx::shared_future<InputElement> const& input_scalar)
            {
                auto const input_partition_server_ptr{
                    hpx::get_ptr(input_partition).get()};
                auto const& input_partition_server{
                    *input_partition_server_ptr};

                auto offset{input_partition_server.offset()};
                InputData input_partition_data{input_partition_server.data()};

                OutputData output_partition_data{
                    input_partition_data.shape()};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
                std::transform(
                    input_partition_data.begin(),
                    input_partition_data.end(),
                    output_partition_data.begin(),
                    [functor, input_element2=input_scalar.get()](
                        InputElement const input_element1)
                    {
                        return functor(input_element1, input_element2);
                    });
#pragma GCC diagnostic pop

                return OutputPartition{
                    hpx::find_here(), offset,
                    std::move(output_partition_data)};
            },

            input_partition,
            input_scalar);
    }

    struct Action:
        hpx::actions::make_action<
            decltype(&binary_local_operation_partition),
            &binary_local_operation_partition,
            Action>
    {};

};


template<
    typename InputPartition,
    typename OutputPartition,
    typename Functor>
class OverloadPicker<
    ElementT<InputPartition>,
    InputPartition,
    OutputPartition,
    Functor>

{

public:

    using InputElement = ElementT<InputPartition>;

    static OutputPartition binary_local_operation_partition(
        hpx::shared_future<InputElement> const input_scalar,
        InputPartition const& input_partition,
        Functor functor)
    {
        using InputData = DataT<InputPartition>;
        using OutputData = DataT<OutputPartition>;

        lue_assert(input_partition.locality_id().get() == hpx::find_here());

        return hpx::dataflow(
            hpx::launch::async,

            [functor](
                hpx::shared_future<InputElement> const& input_scalar,
                InputPartition const& input_partition)
            {
                auto const input_partition_server_ptr{
                    hpx::get_ptr(input_partition).get()};
                auto const& input_partition_server{
                    *input_partition_server_ptr};

                auto offset{input_partition_server.offset()};
                InputData input_partition_data{input_partition_server.data()};

                OutputData output_partition_data{
                    input_partition_data.shape()};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
                std::transform(
                    input_partition_data.begin(),
                    input_partition_data.end(),
                    output_partition_data.begin(),
                    [functor, input_element1=input_scalar.get()](
                        InputElement const input_element2)
                    {
                        return functor(input_element1, input_element2);
                    });
#pragma GCC diagnostic pop

                return OutputPartition{
                    hpx::find_here(), offset,
                    std::move(output_partition_data)};
            },

            input_scalar,
            input_partition);
    }

    struct Action:
        hpx::actions::make_action<
            decltype(&binary_local_operation_partition),
            &binary_local_operation_partition,
            Action>
    {};

};

}  // namespace binary_local_operation


template<
    typename T1,
    typename T2,
    typename OutputPartition,
    typename Functor>
using BinaryLocalOperationPartitionAction =
    typename binary_local_operation::OverloadPicker<
        T1, T2, OutputPartition, Functor>::Action;

}  // namespace detail


// local_operation(partition, scalar)
template<
    typename InputElement,
    Rank rank,
    typename Functor>
ArrayPartition<OutputElementT<Functor>, rank> binary_local_operation(
    ArrayPartition<InputElement, rank> const& input_partition,
    hpx::shared_future<InputElement> const& input_scalar,
    Functor const& functor)
{
    using InputPartition = ArrayPartition<InputElement, rank>;
    using OutputPartition = ArrayPartition<OutputElementT<Functor>, rank>;

    detail::BinaryLocalOperationPartitionAction<
        InputPartition, InputElement, OutputPartition, Functor> action;

    return hpx::dataflow(
        hpx::launch::async,
        hpx::util::unwrapping(

                [action, functor, input_partition, input_scalar](
                    hpx::id_type const locality_id)
                {
                    return action(locality_id,
                        input_partition, input_scalar,
                        functor);
                }

            ),
        input_partition.locality_id());
}


// local_operation(array, array)
template<
    typename InputElement,
    Rank rank,
    typename Functor>
PartitionedArray<OutputElementT<Functor>, rank> binary_local_operation(
    PartitionedArray<InputElement, rank> const& input_array1,
    PartitionedArray<InputElement, rank> const& input_array2,
    Functor const& functor)
{
    using InputArray = PartitionedArray<InputElement, rank>;
    using InputPartition = PartitionT<InputArray>;

    using OutputElement = OutputElementT<Functor>;
    using OutputArray = PartitionedArray<OutputElement, rank>;
    using OutputPartition = PartitionT<OutputArray>;
    using OutputPartitions = PartitionsT<OutputArray>;

    lue_assert(nr_partitions(input_array1) == nr_partitions(input_array2));
    lue_assert(
        shape_in_partitions(input_array1) == shape_in_partitions(input_array2));

    detail::BinaryLocalOperationPartitionAction<
        InputPartition, InputPartition, OutputPartition, Functor> action;
    OutputPartitions output_partitions{shape_in_partitions(input_array1)};

    for(Index p = 0; p < nr_partitions(input_array1); ++p)
    {
        InputPartition const& input_partition1{input_array1.partitions()[p]};
        InputPartition const& input_partition2{input_array2.partitions()[p]};

        output_partitions[p] = hpx::dataflow(
            hpx::launch::async,
            hpx::util::unwrapping(

                    [action, functor, input_partition1, input_partition2](
                        hpx::id_type const locality_id)
                    {
                        return action(locality_id,
                            input_partition1, input_partition2,
                            functor);
                    }

                ),
            input_partition1.locality_id());
    }

    return OutputArray{shape(input_array1), std::move(output_partitions)};
}


// local_operation(array, scalar)
template<
    typename InputElement,
    Rank rank,
    typename Functor>
PartitionedArray<OutputElementT<Functor>, rank> binary_local_operation(
    PartitionedArray<InputElement, rank> const& input_array,
    hpx::shared_future<InputElement> const& input_scalar,
    Functor const& functor)
{
    using InputArray = PartitionedArray<InputElement, rank>;
    using InputPartition = PartitionT<InputArray>;

    using OutputArray = PartitionedArray<OutputElementT<Functor>, rank>;
    using OutputPartition = PartitionT<OutputArray>;
    using OutputPartitions = PartitionsT<OutputArray>;

    detail::BinaryLocalOperationPartitionAction<
        InputPartition, InputElement, OutputPartition, Functor> action;
    OutputPartitions output_partitions{shape_in_partitions(input_array)};

    for(Index p = 0; p < nr_partitions(input_array); ++p)
    {
        InputPartition const& input_partition{input_array.partitions()[p]};

        output_partitions[p] = hpx::dataflow(
            hpx::launch::async,
            hpx::util::unwrapping(

                    [action, functor, input_partition, input_scalar](
                        hpx::id_type const locality_id)
                    {
                        return action(locality_id,
                            input_partition, input_scalar,
                            functor);
                    }

                ),
            input_partition.locality_id());
    }

    return OutputArray{shape(input_array), std::move(output_partitions)};
}


// local_operation(scalar, array)
template<
    typename InputElement,
    Rank rank,
    typename Functor>
PartitionedArray<OutputElementT<Functor>, rank> binary_local_operation(
    hpx::shared_future<InputElement> const& input_scalar,
    PartitionedArray<InputElement, rank> const& input_array,
    Functor const& functor)
{
    using InputArray = PartitionedArray<InputElement, rank>;
    using InputPartition = PartitionT<InputArray>;

    using OutputArray = PartitionedArray<OutputElementT<Functor>, rank>;
    using OutputPartition = PartitionT<OutputArray>;
    using OutputPartitions = PartitionsT<OutputArray>;

    detail::BinaryLocalOperationPartitionAction<
        InputElement, InputPartition, OutputPartition, Functor> action;
    OutputPartitions output_partitions{shape_in_partitions(input_array)};

    for(Index p = 0; p < nr_partitions(input_array); ++p)
    {
        InputPartition const& input_partition{input_array.partitions()[p]};

        output_partitions[p] = hpx::dataflow(
            hpx::launch::async,
            hpx::util::unwrapping(

                    [action, functor, input_scalar, input_partition](
                        hpx::id_type const locality_id)
                    {
                        return action(locality_id,
                            input_scalar, input_partition,
                            functor);
                    }

                ),
            input_partition.locality_id());
    }

    return OutputArray{shape(input_array), std::move(output_partitions)};
}

}  // namespace lue
