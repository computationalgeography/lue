#pragma once
#include "lue/framework/algorithm/functor_traits.hpp"
#include "lue/framework/core/component/partitioned_array.hpp"


namespace lue {
namespace detail {
namespace ternary_local_operation {

template<
    typename Input1,
    typename Input2,
    typename Input3,
    typename OutputPartition,
    typename Functor>
class OverloadPicker
{
};


template<
    typename InputElement1,
    typename InputElement2,
    typename InputElement3,
    typename OutputPartition,
    typename Functor>
class OverloadPicker<
    ArrayPartition<InputElement1, rank<OutputPartition>>,
    ArrayPartition<InputElement2, rank<OutputPartition>>,
    ArrayPartition<InputElement3, rank<OutputPartition>>,
    OutputPartition,
    Functor>

{

public:

    static constexpr Rank rank{lue::rank<OutputPartition>};

    using InputPartition1 = ArrayPartition<InputElement1, rank>;
    using InputPartition2 = ArrayPartition<InputElement2, rank>;
    using InputPartition3 = ArrayPartition<InputElement3, rank>;

    static OutputPartition ternary_local_operation_partition(
        InputPartition1 const& input_partition1,
        InputPartition2 const& input_partition2,
        InputPartition3 const& input_partition3,
        Functor functor)
    {
        lue_assert(input_partition1.locality_id().get() == hpx::find_here());
        lue_assert(input_partition2.locality_id().get() == hpx::find_here());
        lue_assert(input_partition3.locality_id().get() == hpx::find_here());

        using InputData1 = DataT<InputPartition1>;
        using InputData2 = DataT<InputPartition2>;
        using InputData3 = DataT<InputPartition3>;

        using OutputData = DataT<OutputPartition>;

        return hpx::dataflow(
            hpx::launch::async,

            [functor](
                InputPartition1 const& input_partition1,
                InputPartition2 const& input_partition2,
                InputPartition3 const& input_partition3)
            {
                auto const input_partition_server_ptr1{
                    hpx::get_ptr(input_partition1).get()};
                auto const& input_partition_server1{
                    *input_partition_server_ptr1};

                auto const input_partition_server_ptr2{
                    hpx::get_ptr(input_partition2).get()};
                auto const& input_partition_server2{
                    *input_partition_server_ptr2};

                auto const input_partition_server_ptr3{
                    hpx::get_ptr(input_partition3).get()};
                auto const& input_partition_server3{
                    *input_partition_server_ptr3};

                auto offset{input_partition_server1.offset()};
                lue_assert(input_partition_server2.offset() == offset);
                lue_assert(input_partition_server3.offset() == offset);

                InputData1 const input_partition_data1{input_partition_server1.data()};
                InputData2 const input_partition_data2{input_partition_server2.data()};
                InputData3 const input_partition_data3{input_partition_server3.data()};

                lue_assert(
                    input_partition_data1.nr_elements() ==
                    input_partition_data2.nr_elements());
                lue_assert(
                    input_partition_data1.nr_elements() ==
                    input_partition_data3.nr_elements());

                OutputData output_partition_data{input_partition_data1.shape()};

                Count const nr_elements{lue::nr_elements(input_partition_data1)};

                for(Index i = 0; i < nr_elements; ++i) {
                    output_partition_data[i] = functor(
                        input_partition_data1[i],
                        input_partition_data2[i],
                        input_partition_data3[i]);
                }

                return OutputPartition{
                    hpx::find_here(), offset, std::move(output_partition_data)};
            },

            input_partition1,
            input_partition2,
            input_partition3);
    }

    struct Action:
        hpx::actions::make_action<
            decltype(&ternary_local_operation_partition),
            &ternary_local_operation_partition,
            Action>
    {};

};


template<
    typename InputElement1,
    typename InputElement2,
    typename InputElement3,
    typename OutputPartition,
    typename Functor>
class OverloadPicker<
    ArrayPartition<InputElement1, rank<OutputPartition>>,
    ArrayPartition<InputElement2, rank<OutputPartition>>,
    InputElement3,
    OutputPartition,
    Functor>

{

public:

    static constexpr Rank rank{lue::rank<OutputPartition>};

    using InputPartition1 = ArrayPartition<InputElement1, rank>;
    using InputPartition2 = ArrayPartition<InputElement2, rank>;

    static OutputPartition ternary_local_operation_partition(
        InputPartition1 const& input_partition1,
        InputPartition2 const& input_partition2,
        hpx::shared_future<InputElement3> const& input_scalar,
        Functor functor)
    {
        lue_assert(input_partition1.locality_id().get() == hpx::find_here());
        lue_assert(input_partition2.locality_id().get() == hpx::find_here());

        using InputData1 = DataT<InputPartition1>;
        using InputData2 = DataT<InputPartition2>;

        using OutputData = DataT<OutputPartition>;

        return hpx::dataflow(
            hpx::launch::async,

            [functor](
                InputPartition1 const& input_partition1,
                InputPartition2 const& input_partition2,
                hpx::shared_future<InputElement3> const& input_scalar_f)
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

                InputData1 const input_partition_data1{input_partition_server1.data()};
                InputData2 const input_partition_data2{input_partition_server2.data()};
                InputElement3 const input_scalar{input_scalar_f.get()};

                lue_assert(
                    input_partition_data1.nr_elements() ==
                    input_partition_data2.nr_elements());

                OutputData output_partition_data{input_partition_data1.shape()};

                Count const nr_elements{lue::nr_elements(input_partition_data1)};

                for(Index i = 0; i < nr_elements; ++i) {
                    output_partition_data[i] = functor(
                        input_partition_data1[i],
                        input_partition_data2[i],
                        input_scalar);
                }

                return OutputPartition{
                    hpx::find_here(), offset, std::move(output_partition_data)};
            },

            input_partition1,
            input_partition2,
            input_scalar);
    }

    struct Action:
        hpx::actions::make_action<
            decltype(&ternary_local_operation_partition),
            &ternary_local_operation_partition,
            Action>
    {};

};


template<
    typename InputElement1,
    typename InputElement2,
    typename InputElement3,
    typename OutputPartition,
    typename Functor>
class OverloadPicker<
    ArrayPartition<InputElement1, rank<OutputPartition>>,
    InputElement2,
    ArrayPartition<InputElement3, rank<OutputPartition>>,
    OutputPartition,
    Functor>

{

public:

    static constexpr Rank rank{lue::rank<OutputPartition>};

    using InputPartition1 = ArrayPartition<InputElement1, rank>;
    using InputPartition2 = ArrayPartition<InputElement3, rank>;

    static OutputPartition ternary_local_operation_partition(
        InputPartition1 const& input_partition1,
        hpx::shared_future<InputElement2> const& input_scalar,
        InputPartition2 const& input_partition2,
        Functor functor)
    {
        lue_assert(input_partition1.locality_id().get() == hpx::find_here());
        lue_assert(input_partition2.locality_id().get() == hpx::find_here());

        using InputData1 = DataT<InputPartition1>;
        using InputData2 = DataT<InputPartition2>;

        using OutputData = DataT<OutputPartition>;

        return hpx::dataflow(
            hpx::launch::async,

            [functor](
                InputPartition1 const& input_partition1,
                hpx::shared_future<InputElement2> const& input_scalar_f,
                InputPartition2 const& input_partition2)
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

                InputData1 const input_partition_data1{input_partition_server1.data()};
                InputElement2 const input_scalar{input_scalar_f.get()};
                InputData2 const input_partition_data2{input_partition_server2.data()};

                lue_assert(
                    input_partition_data1.nr_elements() ==
                    input_partition_data2.nr_elements());

                OutputData output_partition_data{input_partition_data1.shape()};

                Count const nr_elements{lue::nr_elements(input_partition_data1)};

                for(Index i = 0; i < nr_elements; ++i) {
                    output_partition_data[i] = functor(
                        input_partition_data1[i],
                        input_scalar,
                        input_partition_data2[i]);
                }

                return OutputPartition{
                    hpx::find_here(), offset, std::move(output_partition_data)};
            },

            input_partition1,
            input_scalar,
            input_partition2);
    }

    struct Action:
        hpx::actions::make_action<
            decltype(&ternary_local_operation_partition),
            &ternary_local_operation_partition,
            Action>
    {};

};


template<
    typename InputElement1,
    typename InputElement2,
    typename InputElement3,
    typename OutputPartition,
    typename Functor>
class OverloadPicker<
    ArrayPartition<InputElement1, rank<OutputPartition>>,
    InputElement2,
    InputElement3,
    OutputPartition,
    Functor>

{

public:

    static constexpr Rank rank{lue::rank<OutputPartition>};

    using InputPartition = ArrayPartition<InputElement1, rank>;

    static OutputPartition ternary_local_operation_partition(
        InputPartition const& input_partition,
        hpx::shared_future<InputElement2> const& input_scalar1,
        hpx::shared_future<InputElement3> const& input_scalar2,
        Functor functor)
    {
        lue_assert(input_partition.locality_id().get() == hpx::find_here());

        using InputData = DataT<InputPartition>;

        using OutputData = DataT<OutputPartition>;

        return hpx::dataflow(
            hpx::launch::async,

            [functor](
                InputPartition const& input_partition,
                hpx::shared_future<InputElement2> const& input_scalar_f1,
                hpx::shared_future<InputElement3> const& input_scalar_f2)
            {
                auto const input_partition_server_ptr{
                    hpx::get_ptr(input_partition).get()};
                auto const& input_partition_server{
                    *input_partition_server_ptr};

                auto offset{input_partition_server.offset()};

                InputData const input_partition_data{input_partition_server.data()};
                InputElement2 const input_scalar1{input_scalar_f1.get()};
                InputElement3 const input_scalar2{input_scalar_f2.get()};

                OutputData output_partition_data{input_partition_data.shape()};

                Count const nr_elements{lue::nr_elements(input_partition_data)};

                for(Index i = 0; i < nr_elements; ++i) {
                    output_partition_data[i] = functor(
                        input_partition_data[i],
                        input_scalar1,
                        input_scalar2);
                }

                return OutputPartition{
                    hpx::find_here(), offset, std::move(output_partition_data)};
            },

            input_partition,
            input_scalar1,
            input_scalar2);
    }

    struct Action:
        hpx::actions::make_action<
            decltype(&ternary_local_operation_partition),
            &ternary_local_operation_partition,
            Action>
    {};

};

}  // namespace ternary_local_operation


template<
    typename T1,
    typename T2,
    typename T3,
    typename OutputPartition,
    typename Functor>
using TernaryLocalOperationPartitionAction =
    typename ternary_local_operation::OverloadPicker<
        T1, T2, T3, OutputPartition, Functor>::Action;

}  // namespace detail


// local_operation(array, array, array)
template<
    typename InputElement1,
    typename InputElement2,
    typename InputElement3,
    Rank rank,
    typename Functor>
PartitionedArray<OutputElementT<Functor>, rank> ternary_local_operation(
    PartitionedArray<InputElement1, rank> const& input_array1,
    PartitionedArray<InputElement2, rank> const& input_array2,
    PartitionedArray<InputElement3, rank> const& input_array3,
    Functor const& functor)
{
    using InputArray1 = PartitionedArray<InputElement1, rank>;
    using InputPartition1 = PartitionT<InputArray1>;

    using InputArray2 = PartitionedArray<InputElement2, rank>;
    using InputPartition2 = PartitionT<InputArray2>;

    using InputArray3 = PartitionedArray<InputElement3, rank>;
    using InputPartition3 = PartitionT<InputArray3>;

    using OutputArray = PartitionedArray<OutputElementT<Functor>, rank>;
    using OutputPartition = PartitionT<OutputArray>;
    using OutputPartitions = PartitionsT<OutputArray>;

    lue_assert(nr_partitions(input_array1) == nr_partitions(input_array2));
    lue_assert(nr_partitions(input_array1) == nr_partitions(input_array3));
    lue_assert(
        shape_in_partitions(input_array1) == shape_in_partitions(input_array2));
    lue_assert(
        shape_in_partitions(input_array1) == shape_in_partitions(input_array3));

    detail::TernaryLocalOperationPartitionAction<
        InputPartition1, InputPartition2, InputPartition3, OutputPartition,
        Functor> action;
    OutputPartitions output_partitions{shape_in_partitions(input_array1)};

    for(Index p = 0; p < nr_partitions(input_array1); ++p)
    {
        InputPartition1 const& input_partition1{input_array1.partitions()[p]};
        InputPartition2 const& input_partition2{input_array2.partitions()[p]};
        InputPartition3 const& input_partition3{input_array3.partitions()[p]};

        output_partitions[p] = hpx::dataflow(
            hpx::launch::async,
            hpx::util::unwrapping(

                    [action, functor, input_partition1, input_partition2, input_partition3](
                        hpx::id_type const locality_id)
                    {
                        return action(
                            locality_id,
                            input_partition1, input_partition2, input_partition3,
                            functor);
                    }

                ),
            input_partition1.locality_id());
    }

    return OutputArray{shape(input_array1), std::move(output_partitions)};
}


// local_operation(array, array, scalar)
template<
    typename InputElement1,
    typename InputElement2,
    typename InputElement3,
    Rank rank,
    typename Functor>
PartitionedArray<OutputElementT<Functor>, rank> ternary_local_operation(
    PartitionedArray<InputElement1, rank> const& input_array1,
    PartitionedArray<InputElement2, rank> const& input_array2,
    hpx::shared_future<InputElement3> const& input_scalar,
    Functor const& functor)
{
    using InputArray1 = PartitionedArray<InputElement1, rank>;
    using InputPartition1 = PartitionT<InputArray1>;

    using InputArray2 = PartitionedArray<InputElement2, rank>;
    using InputPartition2 = PartitionT<InputArray2>;

    using OutputArray = PartitionedArray<OutputElementT<Functor>, rank>;
    using OutputPartition = PartitionT<OutputArray>;
    using OutputPartitions = PartitionsT<OutputArray>;

    lue_assert(nr_partitions(input_array1) == nr_partitions(input_array2));
    lue_assert(
        shape_in_partitions(input_array1) == shape_in_partitions(input_array2));

    detail::TernaryLocalOperationPartitionAction<
        InputPartition1, InputPartition2, InputElement3, OutputPartition,
        Functor> action;
    OutputPartitions output_partitions{shape_in_partitions(input_array1)};

    for(Index p = 0; p < nr_partitions(input_array1); ++p)
    {
        InputPartition1 const& input_partition1{input_array1.partitions()[p]};
        InputPartition2 const& input_partition2{input_array2.partitions()[p]};

        output_partitions[p] = hpx::dataflow(
            hpx::launch::async,
            hpx::util::unwrapping(

                    [action, functor, input_partition1, input_partition2, input_scalar](
                        hpx::id_type const locality_id)
                    {
                        return action(
                            locality_id,
                            input_partition1, input_partition2, input_scalar,
                            functor);
                    }

                ),
            input_partition1.locality_id());
    }

    return OutputArray{shape(input_array1), std::move(output_partitions)};
}


// local_operation(array, scalar, array)
template<
    typename InputElement1,
    typename InputElement2,
    typename InputElement3,
    Rank rank,
    typename Functor>
PartitionedArray<OutputElementT<Functor>, rank> ternary_local_operation(
    PartitionedArray<InputElement1, rank> const& input_array1,
    hpx::shared_future<InputElement2> const& input_scalar,
    PartitionedArray<InputElement3, rank> const& input_array2,
    Functor const& functor)
{
    using InputArray1 = PartitionedArray<InputElement1, rank>;
    using InputPartition1 = PartitionT<InputArray1>;

    using InputArray2 = PartitionedArray<InputElement3, rank>;
    using InputPartition2 = PartitionT<InputArray2>;

    using OutputArray = PartitionedArray<OutputElementT<Functor>, rank>;
    using OutputPartition = PartitionT<OutputArray>;
    using OutputPartitions = PartitionsT<OutputArray>;

    lue_assert(nr_partitions(input_array1) == nr_partitions(input_array2));
    lue_assert(
        shape_in_partitions(input_array1) == shape_in_partitions(input_array2));

    detail::TernaryLocalOperationPartitionAction<
        InputPartition1, InputElement2, InputPartition2, OutputPartition,
        Functor> action;
    OutputPartitions output_partitions{shape_in_partitions(input_array1)};

    for(Index p = 0; p < nr_partitions(input_array1); ++p)
    {
        InputPartition1 const& input_partition1{input_array1.partitions()[p]};
        InputPartition2 const& input_partition2{input_array2.partitions()[p]};

        output_partitions[p] = hpx::dataflow(
            hpx::launch::async,
            hpx::util::unwrapping(

                    [action, functor, input_partition1, input_scalar, input_partition2](
                        hpx::id_type const locality_id)
                    {
                        return action(
                            locality_id,
                            input_partition1, input_scalar, input_partition2,
                            functor);
                    }

                ),
            input_partition1.locality_id());
    }

    return OutputArray{shape(input_array1), std::move(output_partitions)};
}


// local_operation(array, scalar, scalar)
template<
    typename InputElement1,
    typename InputElement2,
    typename InputElement3,
    Rank rank,
    typename Functor>
PartitionedArray<OutputElementT<Functor>, rank> ternary_local_operation(
    PartitionedArray<InputElement1, rank> const& input_array,
    hpx::shared_future<InputElement2> const& input_scalar1,
    hpx::shared_future<InputElement3> const& input_scalar2,
    Functor const& functor)
{
    using InputArray = PartitionedArray<InputElement1, rank>;
    using InputPartition = PartitionT<InputArray>;

    using OutputArray = PartitionedArray<OutputElementT<Functor>, rank>;
    using OutputPartition = PartitionT<OutputArray>;
    using OutputPartitions = PartitionsT<OutputArray>;

    detail::TernaryLocalOperationPartitionAction<
        InputPartition, InputElement2, InputElement3, OutputPartition,
        Functor> action;
    OutputPartitions output_partitions{shape_in_partitions(input_array)};

    for(Index p = 0; p < nr_partitions(input_array); ++p)
    {
        InputPartition const& input_partition{input_array.partitions()[p]};

        output_partitions[p] = hpx::dataflow(
            hpx::launch::async,
            hpx::util::unwrapping(

                    [action, functor, input_partition, input_scalar1, input_scalar2](
                        hpx::id_type const locality_id)
                    {
                        return action(
                            locality_id,
                            input_partition, input_scalar1, input_scalar2,
                            functor);
                    }

                ),
            input_partition.locality_id());
    }

    return OutputArray{shape(input_array), std::move(output_partitions)};
}

}  // namespace lue
