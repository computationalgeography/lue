#pragma once
#include "lue/framework/core/component/partitioned_array.hpp"


namespace lue {

// If this is needed elsewhere, put it in algorithm/functor_traits.hpp
// Refactor with unary_local_operation.hpp, focal_operation.hpp
template<
    typename Functor>
using OutputElementT = typename Functor::OutputElement;


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
        InputElement3 const input_element,
        Functor functor)
    {
        assert(
            hpx::get_colocation_id(input_partition1.get_id()).get() ==
            hpx::find_here());
        assert(
            hpx::get_colocation_id(input_partition2.get_id()).get() ==
            hpx::find_here());

        using InputData1 = DataT<InputPartition1>;
        using InputData2 = DataT<InputPartition2>;

        using OutputData = DataT<OutputPartition>;

        auto const input_partition_server_ptr1{
            hpx::get_ptr(input_partition1).get()};
        auto const& input_partition_server1{*input_partition_server_ptr1};

        auto const input_partition_server_ptr2{
            hpx::get_ptr(input_partition2).get()};
        auto const& input_partition_server2{*input_partition_server_ptr2};

        auto offset{input_partition_server1.offset()};
        assert(input_partition_server2.offset() == offset);

        InputData1 input_partition_data1{
            input_partition_server1.data(CopyMode::share)};
        InputData2 input_partition_data2{
            input_partition_server2.data(CopyMode::share)};

        assert(
            input_partition_data1.nr_elements() ==
            input_partition_data2.nr_elements());

        TargetIndex const target_idx = input_partition_data1.target_idx();
        OutputData output_partition_data{
            input_partition_data1.shape(), target_idx};

        Count const nr_elements{lue::nr_elements(input_partition_data1)};

        for(Index i = 0; i < nr_elements; ++i) {
            output_partition_data[i] = functor(
                input_partition_data1[i],
                input_partition_data2[i],
                input_element);
        }

        return OutputPartition{
            hpx::find_here(), offset, std::move(output_partition_data)};

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

    detail::TernaryLocalOperationPartitionAction<
        InputPartition1, InputPartition2, InputElement3, OutputPartition,
        Functor> action;

    OutputPartitions output_partitions{
        shape_in_partitions(input_array1), scattered_target_index()};

    for(Index p = 0; p < nr_partitions(input_array1); ++p) {

        output_partitions[p] = hpx::dataflow(
            hpx::launch::async,

            [action, functor](
                InputPartition1 const& input_partition1,
                InputPartition2 const& input_partition2,
                hpx::shared_future<InputElement3> const& input_scalar,
                hpx::future<hpx::id_type>&& locality_id)
            {
                return action(
                    locality_id.get(),
                    input_partition1,
                    input_partition2,
                    input_scalar.get(),
                    functor);
            },

            input_array1.partitions()[p],
            input_array2.partitions()[p],
            input_scalar,
            hpx::get_colocation_id(input_array1.partitions()[p].get_id()));

    }

    return OutputArray{shape(input_array1), std::move(output_partitions)};
}

}  // namespace lue
