#pragma once
#include "lue/framework/algorithm/functor_traits.hpp"
#include "lue/framework/core/component/partitioned_array.hpp"
#include "lue/framework/core/component.hpp"
// #include "lue/no_data.hpp"


namespace lue {
namespace detail {
namespace binary_local_operation {

template<
    typename T1,
    typename T2,
    typename OutputPartition,
    typename Policies,
    typename Functor,
    typename Enable=void>
class OverloadPicker
{
};


template<
    typename InputPartition,
    typename OutputPartition,
    typename Policies,
    typename Functor>
class OverloadPicker<
    InputPartition,
    InputPartition,
    OutputPartition,
    Policies,
    Functor,
    typename std::enable_if_t<is_array_partition_v<InputPartition>>>

{

public:

    static OutputPartition binary_local_operation_partition(
        Policies const& policies,
        InputPartition const& input_partition1,
        InputPartition const& input_partition2,
        Functor const& functor)
    {
        using Offset = OffsetT<InputPartition>;
        using InputData = DataT<InputPartition>;
        using OutputData = DataT<OutputPartition>;

        lue_assert(input_partition1.is_ready());
        lue_assert(input_partition2.is_ready());

        return hpx::dataflow(
            hpx::launch::async,
            hpx::util::unwrapping(

                    [input_partition1, input_partition2, policies, functor](
                        Offset const& offset,
                        InputData const& input_partition_data1,
                        InputData const& input_partition_data2)
                    {
                        AnnotateFunction annotation{"binary_local_operation_partition"};

                        HPX_UNUSED(input_partition1);
                        HPX_UNUSED(input_partition2);

                        OutputData output_partition_data{input_partition_data1.shape()};

                        auto const& [indp1, indp2] = policies.input_no_data_policies();
                        auto const& [ondp] = policies.output_no_data_policies();

                        Count const nr_elements{lue::nr_elements(input_partition_data1)};
                        lue_assert(lue::nr_elements(input_partition_data2) == nr_elements);

                        for(Index i = 0; i < nr_elements; ++i)
                        {
                            if(indp1.is_no_data(i) || indp2.is_no_data(i))
                            {
                                ondp.mark_no_data(i);
                            }
                            else
                            {
                                output_partition_data[i] =
                                    functor(input_partition_data1[i], input_partition_data2[i]);
                            }
                        }

                        return OutputPartition{hpx::find_here(), offset, std::move(output_partition_data)};
                    }

                ),
            input_partition1.offset(),
            input_partition1.data(),
            input_partition2.data());
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
    typename Policies,
    typename Functor>
class OverloadPicker<
    InputPartition,
    ElementT<InputPartition>,
    OutputPartition,
    Policies,
    Functor>

{

public:

    using InputElement = ElementT<InputPartition>;

    static OutputPartition binary_local_operation_partition(
        Policies const& policies,
        InputPartition const& input_partition,
        InputElement const input_scalar,
        Functor const& functor)
    {
        using Offset = OffsetT<InputPartition>;
        using InputData = DataT<InputPartition>;
        using OutputData = DataT<OutputPartition>;

        lue_assert(input_partition.is_ready());

        return hpx::dataflow(
            hpx::launch::async,
            hpx::util::unwrapping(

                    [input_partition, policies, functor, input_scalar](
                        Offset const& offset,
                        InputData const& input_partition_data)
                    {
                        AnnotateFunction annotation{"binary_local_operation_partition"};

                        HPX_UNUSED(input_partition);

                        OutputData output_partition_data{input_partition_data.shape()};

                        auto const& [indp1, indp2] = policies.input_no_data_policies();
                        auto const& [ondp] = policies.output_no_data_policies();

                        Count const nr_elements{lue::nr_elements(input_partition_data)};

                        if(indp2.is_no_data())
                        {
                            for(Index i = 0; i < nr_elements; ++i)
                            {
                                ondp.mark_no_data(i);
                            }
                        }
                        else
                        {
                            for(Index i = 0; i < nr_elements; ++i)
                            {
                                if(indp1.is_no_data(i))
                                {
                                    ondp.mark_no_data(i);
                                }
                                else
                                {
                                    output_partition_data[i] =
                                        functor(input_partition_data[i], input_scalar);
                                }
                            }
                        }

                        return OutputPartition{hpx::find_here(), offset, std::move(output_partition_data)};
                    }

            ),
            input_partition.offset(),
            input_partition.data());
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
    typename Policies,
    typename Functor>
class OverloadPicker<
    ElementT<InputPartition>,
    InputPartition,
    OutputPartition,
    Policies,
    Functor>

{

public:

    using InputElement = ElementT<InputPartition>;

    static OutputPartition binary_local_operation_partition(
        Policies const& policies,
        InputElement const input_scalar,
        InputPartition const& input_partition,
        Functor const& functor)
    {
        using Offset = OffsetT<InputPartition>;
        using InputData = DataT<InputPartition>;
        using OutputData = DataT<OutputPartition>;

        lue_assert(input_partition.is_ready());

        return hpx::dataflow(
            hpx::launch::async,
            hpx::util::unwrapping(

                    [input_partition, policies, functor, input_scalar](
                        Offset const& offset,
                        InputData const& input_partition_data)
                    {
                        AnnotateFunction annotation{"binary_local_operation_partition"};

                        HPX_UNUSED(input_partition);

                        OutputData output_partition_data{input_partition_data.shape()};

                        auto const& [indp1, indp2] = policies.input_no_data_policies();
                        auto const& [ondp] = policies.output_no_data_policies();

                        Count const nr_elements{lue::nr_elements(input_partition_data)};

                        if(indp1.is_no_data())
                        {
                            for(Index i = 0; i < nr_elements; ++i)
                            {
                                ondp.mark_no_data(i);
                            }
                        }
                        else
                        {
                            for(Index i = 0; i < nr_elements; ++i)
                            {
                                if(indp2.is_no_data(i))
                                {
                                    ondp.mark_no_data(i);
                                }
                                else
                                {
                                    output_partition_data[i] =
                                        functor(input_scalar, input_partition_data[i]);
                                }
                            }
                        }

                        return OutputPartition{hpx::find_here(), offset, std::move(output_partition_data)};
                    }

                ),
            input_partition.offset(),
            input_partition.data());
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
    typename Policies,
    typename T1,
    typename T2,
    typename OutputPartition,
    typename Functor>
using BinaryLocalOperationPartitionAction =
    typename binary_local_operation::OverloadPicker<T1, T2, OutputPartition, Policies, Functor>::Action;

}  // namespace detail


// local_operation(partition, scalar)
template<
    typename Policies,
    typename InputElement,
    Rank rank,
    typename Functor>
ArrayPartition<OutputElementT<Functor>, rank> binary_local_operation(
    hpx::id_type const locality_id,
    Policies const& policies,
    ArrayPartition<InputElement, rank> const& input_partition,
    hpx::shared_future<InputElement> const& input_scalar,
    Functor const& functor)
{
    using InputPartition = ArrayPartition<InputElement, rank>;
    using OutputPartition = ArrayPartition<OutputElementT<Functor>, rank>;

    lue_assert(input_partition.valid());
    lue_assert(input_scalar.valid());

    detail::BinaryLocalOperationPartitionAction<
        Policies, InputPartition, InputElement, OutputPartition, Functor> action;

    return hpx::dataflow(
        hpx::launch::async,

        [locality_id, action, policies, functor](
            InputPartition const& input_partition,
            hpx::shared_future<InputElement> const& input_scalar)
        {
            AnnotateFunction annotation{"binary_local_operation"};

            return action(locality_id, policies, input_partition, input_scalar.get(), functor);
        },

        input_partition,
        input_scalar);
}


// local_operation(array, array)
template<
    typename Policies,
    typename InputElement,
    Rank rank,
    typename Functor>
PartitionedArray<OutputElementT<Functor>, rank> binary_local_operation(
    Policies const& policies,
    PartitionedArray<InputElement, rank> const& input_array1,
    PartitionedArray<InputElement, rank> const& input_array2,
    Functor const& functor)
{
    using InputArray = PartitionedArray<InputElement, rank>;
    using InputPartitions = PartitionsT<InputArray>;
    using InputPartition = PartitionT<InputArray>;

    using OutputElement = OutputElementT<Functor>;
    using OutputArray = PartitionedArray<OutputElement, rank>;
    using OutputPartitions = PartitionsT<OutputArray>;
    using OutputPartition = PartitionT<OutputArray>;

    lue_assert(all_are_valid(input_array1.partitions()));
    lue_assert(all_are_valid(input_array2.partitions()));
    lue_assert(nr_partitions(input_array1) == nr_partitions(input_array2));
    lue_assert(shape_in_partitions(input_array1) == shape_in_partitions(input_array2));

    detail::BinaryLocalOperationPartitionAction<
        Policies, InputPartition, InputPartition, OutputPartition, Functor> action;

    Localities<rank> const& localities{input_array1.localities()};
    InputPartitions const& input_partitions1{input_array1.partitions()};
    InputPartitions const& input_partitions2{input_array2.partitions()};
    OutputPartitions output_partitions{shape_in_partitions(input_array1)};

    for(Index p = 0; p < nr_partitions(input_array1); ++p)
    {
        output_partitions[p] = hpx::dataflow(
            hpx::launch::async,

                [locality_id=localities[p], action, policies, functor](
                    InputPartition const& input_partition1,
                    InputPartition const& input_partition2)
                {
                    AnnotateFunction annotation{"binary_local_operation"};

                    return action(locality_id, policies, input_partition1, input_partition2, functor);
                },

                input_partitions1[p],
                input_partitions2[p]
            );
    }

    return OutputArray{shape(input_array1), localities, std::move(output_partitions)};
}


// local_operation(array, scalar)
template<
    typename Policies,
    typename InputElement,
    Rank rank,
    typename Functor>
PartitionedArray<OutputElementT<Functor>, rank> binary_local_operation(
    Policies const& policies,
    PartitionedArray<InputElement, rank> const& input_array,
    hpx::shared_future<InputElement> const& input_scalar,
    Functor const& functor)
{
    using InputArray = PartitionedArray<InputElement, rank>;
    using InputPartitions = PartitionsT<InputArray>;
    using InputPartition = PartitionT<InputArray>;

    using OutputArray = PartitionedArray<OutputElementT<Functor>, rank>;
    using OutputPartitions = PartitionsT<OutputArray>;
    using OutputPartition = PartitionT<OutputArray>;

    lue_assert(all_are_valid(input_array.partitions()));
    lue_assert(input_scalar.valid());

    detail::BinaryLocalOperationPartitionAction<
        Policies, InputPartition, InputElement, OutputPartition, Functor> action;

    Localities<rank> const& localities{input_array.localities()};
    InputPartitions const& input_partitions{input_array.partitions()};
    OutputPartitions output_partitions{shape_in_partitions(input_array)};

    for(Index p = 0; p < nr_partitions(input_array); ++p)
    {
        output_partitions[p] = hpx::dataflow(
            hpx::launch::async,

                [locality_id=localities[p], action, policies, functor](
                    InputPartition const& input_partition,
                    hpx::shared_future<InputElement> const& input_scalar)
                {
                    AnnotateFunction annotation{"binary_local_operation"};

                    return action(locality_id, policies, input_partition, input_scalar.get(), functor);
                },

                input_partitions[p],
                input_scalar
            );
    }

    return OutputArray{shape(input_array), localities, std::move(output_partitions)};
}


// local_operation(scalar, array)
template<
    typename Policies,
    typename InputElement,
    Rank rank,
    typename Functor>
PartitionedArray<OutputElementT<Functor>, rank> binary_local_operation(
    Policies const& policies,
    hpx::shared_future<InputElement> const& input_scalar,
    PartitionedArray<InputElement, rank> const& input_array,
    Functor const& functor)
{
    using InputArray = PartitionedArray<InputElement, rank>;
    using InputPartitions = PartitionsT<InputArray>;
    using InputPartition = PartitionT<InputArray>;

    using OutputArray = PartitionedArray<OutputElementT<Functor>, rank>;
    using OutputPartitions = PartitionsT<OutputArray>;
    using OutputPartition = PartitionT<OutputArray>;

    lue_assert(input_scalar.valid());
    lue_assert(all_are_valid(input_array.partitions()));

    detail::BinaryLocalOperationPartitionAction<
        Policies, InputElement, InputPartition, OutputPartition, Functor> action;

    Localities<rank> const& localities{input_array.localities()};
    InputPartitions const& input_partitions{input_array.partitions()};
    OutputPartitions output_partitions{shape_in_partitions(input_array)};

    for(Index p = 0; p < nr_partitions(input_array); ++p)
    {
        output_partitions[p] = hpx::dataflow(
            hpx::launch::async,

                [locality_id=localities[p], action, policies, functor](
                    hpx::shared_future<InputElement> const& input_scalar,
                    InputPartition const& input_partition)
                {
                    AnnotateFunction annotation{"binary_local_operation"};

                    return action(locality_id, policies, input_scalar.get(), input_partition, functor);
                },

                input_scalar,
                input_partitions[p]
            );
    }

    return OutputArray{shape(input_array), localities, std::move(output_partitions)};
}

}  // namespace lue
