#pragma once
#include "lue/framework/core/component/array_partition.hpp"
#include <hpx/include/lcos.hpp>


namespace lue {
namespace detail {
namespace equal_to {

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

    static PartitionT<Partition, bool> equal_to_partition(
        Partition const& partition1,
        Partition const& partition2)
    {
        assert(
            hpx::get_colocation_id(partition1.get_id()).get() ==
            hpx::find_here());
        assert(
            hpx::get_colocation_id(partition2.get_id()).get() ==
            hpx::find_here());

        // Obtain the data from both partitions and calculate the values
        // for the new partition

        // Arguments are client instances of components that are ready. Asking
        // for the component values involves copying the data out of the
        // components. In case components lives on the same locality as
        // we do, this copying is not necessary.
        // TODO Optimize for this

        using ResultPartition = PartitionT<Partition, bool>;
        using Data = DataT<Partition>;
        using ResultData = DataT<Partition, bool>;
        using Element = ElementT<Partition>;

        // Asynchronously retrieve the partition data from the array partition
        // components
        hpx::shared_future<Data> partition_data1 =
            partition1.data(CopyMode::share);
        hpx::shared_future<Data> partition_data2 =
            partition2.data(CopyMode::share);

        // Once the data has arrived, compare the values
        hpx::future<ResultData> result_data = hpx::dataflow(
            hpx::launch::async,
            hpx::util::unwrapping([](
                Data const& partition_data1,
                Data const& partition_data2)
            {
                assert(partition_data1.shape() == partition_data2.shape());

                ResultData result{partition_data1.shape()};

                std::transform(
                    partition_data1.begin(),
                    partition_data1.end(),
                    partition_data2.begin(),
                    result.begin(), std::equal_to<Element>{});

                return result;
            }),
            partition_data1,
            partition_data2
        );

        // Once the result has been calculated, create a new component
        // containing the data, on the same locality as the first partition
        // passed in
        return result_data.then(
            // TODO Pass in ref to partition?
            hpx::util::unwrapping([partition1](
                ResultData&& data)
            {
                return ResultPartition(
                    partition1.get_id(), data);
            })
        );
    }

    struct Action:
        hpx::actions::make_action<
            decltype(&equal_to_partition),
            &equal_to_partition,
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

    static PartitionT<Partition, bool> equal_to_partition(
        Partition const& partition,
        hpx::shared_future<ElementT<Partition>> scalar)
    {
        assert(
            hpx::get_colocation_id(partition.get_id()).get() ==
            hpx::find_here());

        using Element = ElementT<Partition>;
        using InputPartition = Partition;
        using InputData = DataT<InputPartition>;

        using ResultPartition = PartitionT<InputPartition, bool>;
        using ResultData = DataT<ResultPartition, bool>;

        // Asynchronously retrieve the partition data from the array partition
        // components
        hpx::shared_future<InputData> partition_data =
            partition.data(CopyMode::share);

        // Once the data has arrived, compare the values
        hpx::future<ResultData> result_data = hpx::dataflow(
            hpx::launch::async,
            hpx::util::unwrapping([](
                InputData const& partition_data,
                Element const scalar)
            {
                ResultData result{partition_data.shape()};

                std::transform(
                    partition_data.begin(), partition_data.end(), result.begin(),
                    [scalar](
                        Element const element)
                    {
                        return element == scalar;
                    });

                return result;
            }),
            partition_data,
            scalar
        );

        // Once the result has been calculated, create a new component
        // containing the data, on the same locality as the first partition
        // passed in
        return result_data.then(
            // TODO Pass in ref to partition?
            hpx::util::unwrapping([partition](
                ResultData&& data)
            {
                return ResultPartition(
                    partition.get_id(), data);
            })
        );
    }

    struct Action:
        hpx::actions::make_action<
            decltype(&equal_to_partition),
            &equal_to_partition,
            Action>
    {};

};

}  // namespace equal_to
}  // namespace detail


template<
    typename T1,
    typename T2>
using EqualToPartitionAction =
    typename detail::equal_to::OverloadPicker<T1, T2>::Action;


template<
    typename Array>
PartitionedArrayT<Array, bool> equal_to(
    Array const& array1,
    Array const& array2)
{
    assert(nr_partitions(array1) == nr_partitions(array2));

    using InputPartition = PartitionT<Array>;

    using OutputArray = PartitionedArrayT<Array, bool>;
    using OutputPartitions = PartitionsT<OutputArray>;
    using OutputPartition = PartitionT<OutputArray>;

    EqualToPartitionAction<InputPartition, InputPartition> action;
    OutputPartitions output_partitions{shape_in_partitions(array1)};

    // Attach a continuation to each pair of input partitions that compares
    // all elements in those partitions and assigns the result to the
    // output partition
    for(std::size_t p = 0; p < nr_partitions(array1); ++p) {

        InputPartition const& input_partition1 = array1.partitions()[p];
        InputPartition const& input_partition2 = array2.partitions()[p];
        OutputPartition& output_partition = output_partitions[p];

        output_partition = hpx::dataflow(
            hpx::launch::async,
            action,
            hpx::get_colocation_id(
                hpx::launch::sync, input_partition1.get_id()),
            input_partition1,
            input_partition2);

    }

    return OutputArray{shape(array1), std::move(output_partitions)};
}


template<
    typename Element,
    std::size_t rank,
    template<typename, std::size_t> typename Array>
PartitionedArrayT<Array<bool, rank>, bool> equal_to(
    Array<Element, rank> const& array,
    hpx::shared_future<Element> const& scalar)
{
    using InputArray = Array<Element, rank>;
    using InputScalar = hpx::shared_future<Element>;
    using InputPartition = PartitionT<InputArray>;

    using OutputArray = PartitionedArrayT<InputArray, bool>;
    using OutputPartitions = PartitionsT<OutputArray>;
    using OutputPartition = PartitionT<OutputArray>;

    EqualToPartitionAction<InputPartition, InputScalar> action;
    OutputPartitions output_partitions{shape_in_partitions(array)};

    // Attach a continuation to each input partition that compares
    // all elements to the scalar and assigns the result to the
    // output partition
    for(std::size_t p = 0; p < nr_partitions(array); ++p) {

        InputPartition const& input_partition = array.partitions()[p];
        OutputPartition& output_partition = output_partitions[p];

        output_partition = hpx::dataflow(
            hpx::launch::async,
            action,
            hpx::get_colocation_id(
                hpx::launch::sync, input_partition.get_id()),
            input_partition,
            scalar);

    }

    return OutputArray{shape(array), std::move(output_partitions)};
}


template<
    typename Element,
    std::size_t rank,
    template<typename, std::size_t> typename Array>
PartitionedArrayT<Array<Element, rank>, bool> equal_to(
    hpx::shared_future<Element> const& scalar,
    Array<Element, rank> const& array)
{
    return equal_to(array, scalar);
}

}  // namespace lue
