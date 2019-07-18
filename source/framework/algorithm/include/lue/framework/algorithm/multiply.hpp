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
        // Obtain the data from both partitions and calculate the values
        // for the new partition

        // Arguments are client instances of components that are ready. Asking
        // for the component values involves copying the data out of the
        // components. In case components lives on the same locality as
        // we do, this copying is not necessary.
        // TODO Optimize for this

        assert(
            hpx::get_colocation_id(partition1.get_id()).get() ==
            hpx::find_here());
        assert(
            hpx::get_colocation_id(partition2.get_id()).get() ==
            hpx::find_here());

        using Data = DataT<Partition>;
        using Element = ElementT<Partition>;

        // Asynchronously retrieve the partition data from the array
        // partition components
        hpx::shared_future<Data> partition_data1 =
            partition1.data(CopyMode::share);
        hpx::shared_future<Data> partition_data2 =
            partition2.data(CopyMode::share);

        // Once the data has arrived, multiply the values
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

        // Once the multiplication has been calculated, create a new component
        // containing the result, on the same locality as the first partition
        // passed in
        hpx::id_type partition_id = partition1.get_id();

        return multiplication.then(
            hpx::util::unwrapping(
                [partition_id](
                    Data&& multiplication_data)
                {
                    return Partition(partition_id, multiplication_data);
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

        // Asynchronously retrieve the partition data from the array
        // partition component
        hpx::shared_future<InputData> input_partition_data =
            partition.data(CopyMode::share);

        // Once the data has arrived, compare the values
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
                            // return
                            //     scalar +
                            //     std::cos(input_element) +
                            //     std::sin(input_element) +
                            //     std::tan(input_element) +
                            //     std::log(input_element) +
                            //     std::exp(input_element) +
                            //     std::cos(input_element) +
                            //     std::sin(input_element) +
                            //     std::tan(input_element) +
                            //     std::log(input_element) +
                            //     std::exp(input_element) +
                            //     std::cos(input_element) +
                            //     std::sin(input_element) +
                            //     std::tan(input_element) +
                            //     std::log(input_element) +
                            //     std::exp(input_element) +
                            //     std::cos(input_element) +
                            //     std::sin(input_element) +
                            //     std::tan(input_element) +
                            //     std::log(input_element) +
                            //     std::exp(input_element) +
                            //     std::cos(input_element) +
                            //     std::sin(input_element) +
                            //     std::tan(input_element) +
                            //     std::log(input_element) +
                            //     std::exp(input_element)
                            //     ;
                        });

                    return result;
                }
            ),
            input_partition_data,
            scalar
        );

        // Once the result has been calculated, create a new component
        // containing the data, on the same locality as the partition
        // passed in
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


// template<
//     typename Partition>
// struct MultiplyPartitionAction:
//     hpx::actions::make_action<
//         decltype(&detail::multiply_partition<Partition>),
//         &detail::multiply_partition<Partition>,
//         MultiplyPartitionAction<Partition>>
// {};


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

    // Attach a continuation to each pair of input partitions that multiplies
    // all elements in those partitions and assigns the result to the
    // output partition
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
    using OutputPartition = PartitionT<OutputArray>;

    MultiplyPartitionAction<InputPartition, InputScalar> action;

    OutputPartitions output_partitions{shape_in_partitions(array)};

    // Attach a continuation to each input partition, that multiplies
    // all elements to the scalar and assigns the result to the
    // output partition
    for(std::size_t p = 0; p < nr_partitions(array); ++p) {

        InputPartition const& input_partition = array.partitions()[p];
        OutputPartition& output_partition = output_partitions[p];

        output_partition =
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
