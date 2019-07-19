#pragma once
#include "lue/framework/core/component/array_partition.hpp"
#include <hpx/include/lcos.hpp>


namespace lue {
namespace detail {
namespace multiply {

template<
    typename T1,
    typename T2,
    typename Enable=void>
class OverloadPicker
{
};


template<
    typename Partition>
class OverloadPicker<
    Partition,
    Partition,
    typename std::enable_if_t<is_array_partition_v<Partition>>>

{

public:

    static Partition multiply_partition(
        Partition const& partition1,
        Partition const& partition2)
    {
        assert(
            hpx::get_colocation_id(partition1.get_id()).get() ==
            hpx::find_here());
        assert(
            hpx::get_colocation_id(partition2.get_id()).get() ==
            hpx::find_here());

        using Data = DataT<Partition>;
        using Element = ElementT<Partition>;

        hpx::shared_future<Data> partition_data1 =
            partition1.data(CopyMode::share);
        hpx::shared_future<Data> partition_data2 =
            partition2.data(CopyMode::share);

        hpx::future<Data> multiplication = hpx::dataflow(
            hpx::launch::async,
            hpx::util::unwrapping(
                [](
                    Data const& partition_data1,
                    Data const& partition_data2)
                {
                    assert(partition_data1.shape() == partition_data2.shape());

                    Data result{partition_data1.shape()};

                    std::transform(
                        partition_data1.begin(),
                        partition_data1.end(),
                        partition_data2.begin(),
                        result.begin(),
                        [](Element const lhs, Element const rhs)
                        {
                            return lhs * rhs;
                        });

                    return result;
                }
            ),
            partition_data1,
            partition_data2
        );

        hpx::id_type partition_id = partition1.get_id();

        return multiplication.then(
            hpx::util::unwrapping(
                [partition_id](
                    Data&& multiplication_data)
                {
                    return Partition{partition_id, multiplication_data};
                }
            )
        );
    }

    struct Action:
        hpx::actions::make_action<
            decltype(&multiply_partition),
            &multiply_partition,
            Action>
    {};

};


template<
    typename Partition>
class OverloadPicker<
    Partition,
    hpx::shared_future<ElementT<Partition>>>

{

public:

    static Partition multiply_partition(
        Partition const& partition,
        hpx::shared_future<ElementT<Partition>> const& scalar)
    {
        assert(
            hpx::get_colocation_id(partition.get_id()).get() ==
            hpx::find_here());

        using InputElement = ElementT<Partition>;
        using InputPartition = Partition;
        using InputData = DataT<InputPartition>;

        using OutputPartition = InputPartition;
        using OutputData = InputData;

        hpx::shared_future<InputData> input_partition_data =
            partition.data(CopyMode::share);

        hpx::future<OutputData> result_data = hpx::dataflow(
            hpx::launch::async,
            hpx::util::unwrapping(
                [](
                    InputData const& input_partition_data,
                    InputElement const scalar)
                {
                    OutputData result{input_partition_data.shape()};

                    std::transform(
                        input_partition_data.begin(),
                        input_partition_data.end(), result.begin(),
                        [scalar](
                            InputElement const input_element)
                        {
                            return input_element * scalar;
                        });

                    return result;
                }
            ),
            input_partition_data,
            scalar
        );

        hpx::id_type partition_id = partition.get_id();

        return result_data.then(
            hpx::util::unwrapping(
                [partition_id](
                    OutputData&& data)
                {
                    return OutputPartition(partition_id, data);
                }
            )
        );
    }

    struct Action:
        hpx::actions::make_action<
            decltype(&multiply_partition),
            &multiply_partition,
            Action>
    {};

};

}  // namespace multiply
}  // namespace detail


template<
    typename T1,
    typename T2>
using MultiplyPartitionAction =
    typename detail::multiply::OverloadPicker<T1, T2>::Action;


template<
    typename Array>
Array multiply(
    Array const& array1,
    Array const& array2)
{
    assert(nr_partitions(array1) == nr_partitions(array2));

    using Partitions = PartitionsT<Array>;
    using Partition = PartitionT<Array>;

    MultiplyPartitionAction<Partition, Partition> action;

    Partitions partitions{shape_in_partitions(array1)};

    for(std::size_t p = 0; p < nr_partitions(array1); ++p) {

        Partition const& input_partition1 = array1.partitions()[p];
        Partition const& input_partition2 = array2.partitions()[p];
        Partition& output_partition = partitions[p];

        output_partition =
            hpx::get_colocation_id(input_partition1.get_id()).then(
                hpx::util::unwrapping(
                    [=](
                        hpx::naming::id_type const locality_id)
                    {
                        return hpx::dataflow(
                            hpx::launch::async,
                            action,
                            locality_id,
                            input_partition1,
                            input_partition2);
                    }
                )
            );

    }

    return Array{shape(array1), std::move(partitions)};
}


template<
    typename Element,
    std::size_t rank,
    template<typename, std::size_t> typename Array>
Array<Element, rank> multiply(
    Array<Element, rank> const& array,
    hpx::shared_future<Element> const& scalar)
{
    using InputScalar = hpx::shared_future<Element>;
    using InputArray = Array<Element, rank>;
    using InputPartition = PartitionT<InputArray>;

    using OutputArray = Array<Element, rank>;
    using OutputPartitions = PartitionsT<OutputArray>;

    MultiplyPartitionAction<InputPartition, InputScalar> action;
    OutputPartitions output_partitions{shape_in_partitions(array)};

    for(std::size_t p = 0; p < nr_partitions(array); ++p) {

        InputPartition const& input_partition = array.partitions()[p];

        output_partitions[p] =
            hpx::get_colocation_id(input_partition.get_id()).then(
                hpx::util::unwrapping(
                    [=](
                        hpx::naming::id_type const locality_id)
                    {
                        return hpx::dataflow(
                            hpx::launch::async,
                            action,
                            locality_id,
                            input_partition,
                            scalar);
                    }
                )
            );
    }

    return OutputArray{shape(array), std::move(output_partitions)};
}


template<
    typename Element,
    std::size_t rank,
    template<typename, std::size_t> typename Array>
Array<Element, rank> multiply(
    hpx::shared_future<Element> const& scalar,
    Array<Element, rank> const& array)
{
    return multiply(array, scalar);
}

}  // namespace lue
