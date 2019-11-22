#pragma once
#include "lue/framework/core/type_traits.hpp"
#include <hpx/include/lcos.hpp>

// Noncommutative


namespace lue {
namespace detail {
namespace less_equal {

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

    static PartitionT<Partition, bool> less_equal_partition(
        Partition const& partition1,
        Partition const& partition2)
    {
        assert(
            hpx::get_colocation_id(partition1.get_id()).get() ==
            hpx::find_here());
        assert(
            hpx::get_colocation_id(partition2.get_id()).get() ==
            hpx::find_here());

        using Element = ElementT<Partition>;
        using InputPartition = Partition;
        using InputData = DataT<InputPartition>;

        using OutputPartition = PartitionT<InputPartition, bool>;
        using OutputData = DataT<OutputPartition>;

        return hpx::dataflow(
            hpx::launch::async,
            hpx::util::unwrapping(

                [](
                    hpx::id_type const locality_id,
                    InputData&& partition_data1,
                    InputData&& partition_data2)
                {
                    assert(partition_data1.shape() == partition_data2.shape());

                    OutputData result{partition_data1.shape()};

                    std::transform(
                        partition_data1.begin(), partition_data1.end(),
                        partition_data2.begin(), result.begin(),
                        [](
                            Element const lhs,
                            Element const rhs)
                        {
                            return lhs <= rhs;
                        });

                    return OutputPartition{locality_id, std::move(result)};
                }

            ),
            hpx::get_colocation_id(partition1.get_id()),
            partition1.data(CopyMode::share),
            partition2.data(CopyMode::share));
    }

    struct Action:
        hpx::actions::make_action<
            decltype(&less_equal_partition),
            &less_equal_partition,
            Action>
    {};

};


template<
    typename Partition>
class OverloadPicker<
    Partition,
    ElementT<Partition>>

{

public:

    static PartitionT<Partition, bool> less_equal_partition(
        Partition const& partition,
        ElementT<Partition> const scalar)
    {
        assert(
            hpx::get_colocation_id(partition.get_id()).get() ==
            hpx::find_here());

        using Element = ElementT<Partition>;
        using InputPartition = Partition;
        using InputData = DataT<InputPartition>;

        using OutputPartition = PartitionT<InputPartition, bool>;
        using OutputData = DataT<OutputPartition, bool>;

        return hpx::dataflow(
            hpx::launch::async,
            hpx::util::unwrapping(

                [scalar](
                    hpx::id_type const locality_id,
                    InputData&& partition_data)
                {
                    OutputData result{partition_data.shape()};

                    std::transform(
                        partition_data.begin(), partition_data.end(),
                        result.begin(),
                        [scalar](
                            Element const element)
                        {
                            return element <= scalar;
                        });

                    return OutputPartition{locality_id, std::move(result)};
                }

            ),
            hpx::get_colocation_id(partition.get_id()),
            partition.data(CopyMode::share));
    }

    struct Action:
        hpx::actions::make_action<
            decltype(&less_equal_partition),
            &less_equal_partition,
            Action>
    {};

};


template<
    typename Partition>
class OverloadPicker<
    ElementT<Partition>,
    Partition>

{

public:

    static PartitionT<Partition, bool> less_equal_partition(
        ElementT<Partition> const scalar,
        Partition const& partition)
    {
        assert(
            hpx::get_colocation_id(partition.get_id()).get() ==
            hpx::find_here());

        using Element = ElementT<Partition>;
        using InputPartition = Partition;
        using InputData = DataT<InputPartition>;

        using OutputPartition = PartitionT<InputPartition, bool>;
        using OutputData = DataT<OutputPartition, bool>;

        return hpx::dataflow(
            hpx::launch::async,
            hpx::util::unwrapping(

                [scalar](
                    hpx::id_type const locality_id,
                    InputData&& partition_data)
                {
                    OutputData result{partition_data.shape()};

                    std::transform(
                        partition_data.begin(), partition_data.end(),
                        result.begin(),
                        [scalar](
                            Element const element)
                        {
                            return scalar <= element;
                        });

                    return OutputPartition{locality_id, std::move(result)};
                }

            ),
            hpx::get_colocation_id(partition.get_id()),
            partition.data(CopyMode::share));
    }

    struct Action:
        hpx::actions::make_action<
            decltype(&less_equal_partition),
            &less_equal_partition,
            Action>
    {};

};

}  // namespace less_equal
}  // namespace detail


template<
    typename T1,
    typename T2>
using LessEqualPartitionAction =
    typename detail::less_equal::OverloadPicker<T1, T2>::Action;


/*!
    @brief      Return the result of comparing two partitioned arrays
    @tparam     Element Type of elements in the arrays
    @tparam     rank Rank of the input arrays
    @tparam     Array Class template of the type of the arrays
    @param      array1 Partitioned array
    @param      array2 Partitioned array
    @return     New partitioned array
*/
template<
    typename Element,
    Rank rank,
    template<typename, Rank> typename Array>
PartitionedArrayT<Array<bool, rank>, bool> less_equal(
    Array<Element, rank> const& array1,
    Array<Element, rank> const& array2)
{
    assert(nr_partitions(array1) == nr_partitions(array2));

    using InputArray = Array<Element, rank>;
    using InputPartition = PartitionT<InputArray>;

    using OutputArray = PartitionedArrayT<InputArray, bool>;
    using OutputPartitions = PartitionsT<OutputArray>;

    LessEqualPartitionAction<InputPartition, InputPartition> action;
    OutputPartitions output_partitions{shape_in_partitions(array1)};

    for(Index p = 0; p < nr_partitions(array1); ++p) {

        output_partitions[p] = hpx::dataflow(
            hpx::launch::async,

            [action](
                InputPartition const& input_partition1,
                InputPartition const& input_partition2)
            {
                return action(
                    hpx::get_colocation_id(
                        hpx::launch::sync, input_partition1.get_id()),
                    input_partition1,
                    input_partition2);
            },

            array1.partitions()[p],
            array2.partitions()[p]);

    }

    return OutputArray{shape(array1), std::move(output_partitions)};
}


/*!
    @overload
*/
template<
    typename Element,
    Rank rank,
    template<typename, Rank> typename Array>
PartitionedArrayT<Array<bool, rank>, bool> less_equal(
    Array<Element, rank> const& array,
    hpx::shared_future<Element> const& scalar)
{
    using InputArray = Array<Element, rank>;
    using InputScalar = hpx::shared_future<Element>;
    using InputPartition = PartitionT<InputArray>;

    using OutputArray = PartitionedArrayT<InputArray, bool>;
    using OutputPartitions = PartitionsT<OutputArray>;

    LessEqualPartitionAction<InputPartition, Element> action;
    OutputPartitions output_partitions{shape_in_partitions(array)};

    for(Index p = 0; p < nr_partitions(array); ++p) {

        output_partitions[p] = hpx::dataflow(
            hpx::launch::async,

            [action](
                InputPartition const& input_partition,
                hpx::shared_future<Element> const& scalar)
            {
                return action(
                    hpx::get_colocation_id(
                        hpx::launch::sync, input_partition.get_id()),
                    input_partition,
                    scalar.get());
            },

            array.partitions()[p],
            scalar);

    }

    return OutputArray{shape(array), std::move(output_partitions)};
}


/*!
    @overload
*/
template<
    typename Element,
    Rank rank,
    template<typename, Rank> typename Array>
PartitionedArrayT<Array<Element, rank>, bool> less_equal(
    hpx::shared_future<Element> const& scalar,
    Array<Element, rank> const& array)
{
    using InputArray = Array<Element, rank>;
    using InputScalar = hpx::shared_future<Element>;
    using InputPartition = PartitionT<InputArray>;

    using OutputArray = PartitionedArrayT<InputArray, bool>;
    using OutputPartitions = PartitionsT<OutputArray>;

    LessEqualPartitionAction<Element, InputPartition> action;
    OutputPartitions output_partitions{shape_in_partitions(array)};

    for(Index p = 0; p < nr_partitions(array); ++p) {

        output_partitions[p] = hpx::dataflow(
            hpx::launch::async,

            [action](
                hpx::shared_future<Element> const& scalar,
                InputPartition const& input_partition)
            {
                return action(
                    hpx::get_colocation_id(
                        hpx::launch::sync, input_partition.get_id()),
                    scalar.get(),
                    input_partition);
            },

            scalar,
            array.partitions()[p]);

    }

    return OutputArray{shape(array), std::move(output_partitions)};
}

}  // namespace lue
