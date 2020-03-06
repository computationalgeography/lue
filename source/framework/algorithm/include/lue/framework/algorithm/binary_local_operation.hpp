#pragma once
#include "lue/framework/core/component/partitioned_array.hpp"


namespace lue {

// If this is needed elsewhere, put it in algorithm/functor_traits.hpp
// Refactor with unary_local_operation.hpp, focal_operation.hpp
template<
    typename Functor>
using OutputElementT = typename Functor::OutputElement;


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
        assert(
            hpx::get_colocation_id(input_partition1.get_id()).get() ==
            hpx::find_here());
        assert(
            hpx::get_colocation_id(input_partition2.get_id()).get() ==
            hpx::find_here());
        assert(
            input_partition1.offset().get() ==
            input_partition2.offset().get());

        using InputData = DataT<InputPartition>;

        using OutputData = DataT<OutputPartition>;

        auto const input_partition_server_ptr1{
            hpx::get_ptr(input_partition1).get()};
        auto const& input_partition_server1{*input_partition_server_ptr1};

        auto const input_partition_server_ptr2{
            hpx::get_ptr(input_partition2).get()};
        auto const& input_partition_server2{*input_partition_server_ptr2};

        auto offset{input_partition_server1.offset()};
        assert(input_partition_server2.offset() == offset);

        InputData input_partition_data1{
            input_partition_server1.data(CopyMode::share)};
        InputData input_partition_data2{
            input_partition_server2.data(CopyMode::share)};

        TargetIndex const target_idx = input_partition_data1.target_idx();
        OutputData output_partition_data{
            input_partition_data1.shape(), target_idx};

        std::transform(
            input_partition_data1.begin(),
            input_partition_data1.end(),
            input_partition_data2.begin(),
            output_partition_data.begin(),
            functor);

        return OutputPartition{
            hpx::find_here(), offset, std::move(output_partition_data)};

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
        InputElement const input_scalar,
        Functor functor)
    {
        assert(
            hpx::get_colocation_id(input_partition.get_id()).get() ==
            hpx::find_here());

        using InputData = DataT<InputPartition>;

        using OutputData = DataT<OutputPartition>;

        auto const input_partition_server_ptr{
            hpx::get_ptr(input_partition).get()};
        auto const& input_partition_server{*input_partition_server_ptr};

        auto offset{input_partition_server.offset()};
        InputData input_partition_data{
            input_partition_server.data(CopyMode::share)};

        TargetIndex const target_idx = input_partition_data.target_idx();
        OutputData output_partition_data{
            input_partition_data.shape(), target_idx};

        std::transform(
            input_partition_data.begin(),
            input_partition_data.end(),
            output_partition_data.begin(),
            [input_scalar, functor](
                InputElement const input_element)
            {
                return functor(input_element, input_scalar);
            });

        return OutputPartition{
            hpx::find_here(), offset, std::move(output_partition_data)};

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
        InputElement const input_scalar,
        InputPartition const& input_partition,
        Functor functor)
    {
        assert(
            hpx::get_colocation_id(input_partition.get_id()).get() ==
            hpx::find_here());

        using InputData = DataT<InputPartition>;

        using OutputData = DataT<OutputPartition>;

        auto const input_partition_server_ptr{
            hpx::get_ptr(input_partition).get()};
        auto const& input_partition_server{*input_partition_server_ptr};

        auto offset{input_partition_server.offset()};
        InputData input_partition_data{
            input_partition_server.data(CopyMode::share)};

        TargetIndex const target_idx = input_partition_data.target_idx();
        OutputData output_partition_data{
            input_partition_data.shape(), target_idx};

        std::transform(
            input_partition_data.begin(),
            input_partition_data.end(),
            output_partition_data.begin(),
            [input_scalar, functor](
                InputElement const input_element)
            {
                return functor(input_scalar, input_element);
            });

        return OutputPartition{
            hpx::find_here(), offset, std::move(output_partition_data)};

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
    typename binary_local_operation::OverloadPicker<T1, T2, OutputPartition, Functor>::Action;

}  // namespace detail


template<
    typename InputElement,
    Rank rank,
    typename Functor>
PartitionedArray<OutputElementT<Functor>, rank> binary_local_operation(
    PartitionedArray<InputElement, rank> const& input_array1,
    PartitionedArray<InputElement, rank> const& input_array2,
    Functor const& functor)
{
    assert(nr_partitions(input_array1) == nr_partitions(input_array2));
    assert(shape_in_partitions(input_array1) == shape_in_partitions(input_array2));

    using InputArray = PartitionedArray<InputElement, rank>;
    using InputPartition = PartitionT<InputArray>;

    using OutputElement = OutputElementT<Functor>;
    using OutputArray = PartitionedArray<OutputElement, rank>;
    using OutputPartition = PartitionT<OutputArray>;
    using OutputPartitions = PartitionsT<OutputArray>;

    detail::BinaryLocalOperationPartitionAction<
        InputPartition, InputPartition, OutputPartition, Functor> action;
    OutputPartitions output_partitions{
        shape_in_partitions(input_array1), scattered_target_index()};

    for(Index p = 0; p < nr_partitions(input_array1); ++p) {

        output_partitions[p] = hpx::dataflow(
            hpx::launch::async,

            [action, functor](
                InputPartition const& input_partition1,
                InputPartition const& input_partition2,
                hpx::future<hpx::id_type>&& locality_id)
            {
                return action(
                    locality_id.get(),
                    input_partition1,
                    input_partition2,
                    functor);
            },

            input_array1.partitions()[p],
            input_array2.partitions()[p],
            hpx::get_colocation_id(input_array1.partitions()[p].get_id()));

    }

    return OutputArray{shape(input_array1), std::move(output_partitions)};
}


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
    OutputPartitions output_partitions{
        shape_in_partitions(input_array), scattered_target_index()};

    for(Index p = 0; p < nr_partitions(input_array); ++p) {

        output_partitions[p] = hpx::dataflow(
            hpx::launch::async,

            [action, functor](
                InputPartition const& input_partition,
                hpx::shared_future<InputElement> const& input_scalar,
                hpx::future<hpx::id_type>&& locality_id)
            {
                return action(
                    locality_id.get(),
                    input_partition,
                    input_scalar.get(),
                    functor);
            },

            input_array.partitions()[p],
            input_scalar,
            hpx::get_colocation_id(input_array.partitions()[p].get_id()));

    }

    return OutputArray{shape(input_array), std::move(output_partitions)};
}


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
    OutputPartitions output_partitions{
        shape_in_partitions(input_array), scattered_target_index()};

    for(Index p = 0; p < nr_partitions(input_array); ++p) {

        output_partitions[p] = hpx::dataflow(
            hpx::launch::async,

            [action, functor](
                hpx::shared_future<InputElement> const& input_scalar,
                InputPartition const& input_partition,
                hpx::future<hpx::id_type>&& locality_id)
            {
                return action(
                    locality_id.get(),
                    input_scalar.get(),
                    input_partition,
                    functor);
            },

            input_scalar,
            input_array.partitions()[p],
            hpx::get_colocation_id(input_array.partitions()[p].get_id()));

    }

    return OutputArray{shape(input_array), std::move(output_partitions)};
}

}  // namespace lue
