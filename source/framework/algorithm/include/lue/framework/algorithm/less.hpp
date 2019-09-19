#pragma once
#include "lue/framework/core/type_traits.hpp"
#include <hpx/include/lcos.hpp>

// Noncommutative


namespace lue {
namespace detail {
namespace less {

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

    static PartitionT<Partition, bool> less_partition(
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

        hpx::shared_future<InputData> partition_data1 =
            partition1.data(CopyMode::share);
        hpx::shared_future<InputData> partition_data2 =
            partition2.data(CopyMode::share);

        hpx::future<OutputData> result_data = hpx::dataflow(
            hpx::launch::async,
            hpx::util::unwrapping(
                [](
                    InputData const& partition_data1,
                    InputData const& partition_data2)
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
                            return lhs < rhs;
                        });

                    return result;
                }
            ),
            partition_data1,
            partition_data2
        );

        return hpx::when_all(
                hpx::get_colocation_id(partition1.get_id()), result_data)
            .then(
                hpx::util::unwrapping(
                    [](
                        auto&& futures)
                    {
                        auto const locality_id = hpx::util::get<0>(futures).get();
                        auto&& data = hpx::util::get<1>(futures).get();

                        return OutputPartition{locality_id, std::move(data)};
                    }
                )
            );
    }

    struct Action:
        hpx::actions::make_action<
            decltype(&less_partition),
            &less_partition,
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

    static PartitionT<Partition, bool> less_partition(
        Partition const& partition,
        hpx::shared_future<ElementT<Partition>> scalar)
    {
        assert(
            hpx::get_colocation_id(partition.get_id()).get() ==
            hpx::find_here());

        using Element = ElementT<Partition>;
        using InputPartition = Partition;
        using InputData = DataT<InputPartition>;

        using OutputPartition = PartitionT<InputPartition, bool>;
        using OutputData = DataT<OutputPartition, bool>;

        hpx::shared_future<InputData> partition_data =
            partition.data(CopyMode::share);

        hpx::future<OutputData> result_data = hpx::dataflow(
            hpx::launch::async,
            hpx::util::unwrapping(
                [](
                    InputData const& partition_data,
                    Element const scalar)
                {
                    OutputData result{partition_data.shape()};

                    std::transform(
                        partition_data.begin(), partition_data.end(),
                        result.begin(),
                        [scalar](
                            Element const element)
                        {
                            return element < scalar;
                        });

                    return result;
                }
            ),
            partition_data,
            scalar
        );

        return hpx::when_all(
                hpx::get_colocation_id(partition.get_id()), result_data)
            .then(
                hpx::util::unwrapping(
                    [](
                        auto&& futures)
                    {
                        auto const locality_id = hpx::util::get<0>(futures).get();
                        auto&& data = hpx::util::get<1>(futures).get();

                        return OutputPartition{locality_id, std::move(data)};
                    }
                )
            );
    }

    struct Action:
        hpx::actions::make_action<
            decltype(&less_partition),
            &less_partition,
            Action>
    {};

};


template<
    typename Partition>
class OverloadPicker<
    hpx::shared_future<ElementT<Partition>>,
    Partition>

{

public:

    static PartitionT<Partition, bool> less_partition(
        hpx::shared_future<ElementT<Partition>> scalar,
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

        hpx::shared_future<InputData> partition_data =
            partition.data(CopyMode::share);

        hpx::future<OutputData> result_data = hpx::dataflow(
            hpx::launch::async,
            hpx::util::unwrapping(
                [](
                    Element const scalar,
                    InputData const& partition_data)
                {
                    OutputData result{partition_data.shape()};

                    std::transform(
                        partition_data.begin(), partition_data.end(),
                        result.begin(),
                        [scalar](
                            Element const element)
                        {
                            return scalar < element;
                        });

                    return result;
                }
            ),
            scalar,
            partition_data
        );

        return hpx::when_all(
                hpx::get_colocation_id(partition.get_id()), result_data)
            .then(
                hpx::util::unwrapping(
                    [](
                        auto&& futures)
                    {
                        auto const locality_id = hpx::util::get<0>(futures).get();
                        auto&& data = hpx::util::get<1>(futures).get();

                        return OutputPartition{locality_id, std::move(data)};
                    }
                )
            );
    }

    struct Action:
        hpx::actions::make_action<
            decltype(&less_partition),
            &less_partition,
            Action>
    {};

};

}  // namespace less
}  // namespace detail


template<
    typename T1,
    typename T2>
using LessPartitionAction =
    typename detail::less::OverloadPicker<T1, T2>::Action;


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
    std::size_t rank,
    template<typename, std::size_t> typename Array>
PartitionedArrayT<Array<bool, rank>, bool> less(
    Array<Element, rank> const& array1,
    Array<Element, rank> const& array2)
{
    assert(nr_partitions(array1) == nr_partitions(array2));

    using InputArray = Array<Element, rank>;
    using InputPartition = PartitionT<InputArray>;

    using OutputArray = PartitionedArrayT<InputArray, bool>;
    using OutputPartitions = PartitionsT<OutputArray>;

    LessPartitionAction<InputPartition, InputPartition> action;
    OutputPartitions output_partitions{shape_in_partitions(array1)};

    for(std::size_t p = 0; p < nr_partitions(array1); ++p) {

        InputPartition const& input_partition1 = array1.partitions()[p];
        InputPartition const& input_partition2 = array2.partitions()[p];

        output_partitions[p] =
            hpx::get_colocation_id(input_partition1.get_id()).then(
                hpx::util::unwrapping(
                    [=](
                        hpx::id_type const locality_id)
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

    return OutputArray{shape(array1), std::move(output_partitions)};
}


/*!
    @overload
*/
template<
    typename Element,
    std::size_t rank,
    template<typename, std::size_t> typename Array>
PartitionedArrayT<Array<bool, rank>, bool> less(
    Array<Element, rank> const& array,
    hpx::shared_future<Element> const& scalar)
{
    using InputArray = Array<Element, rank>;
    using InputScalar = hpx::shared_future<Element>;
    using InputPartition = PartitionT<InputArray>;

    using OutputArray = PartitionedArrayT<InputArray, bool>;
    using OutputPartitions = PartitionsT<OutputArray>;

    LessPartitionAction<InputPartition, InputScalar> action;
    OutputPartitions output_partitions{shape_in_partitions(array)};

    for(std::size_t p = 0; p < nr_partitions(array); ++p) {

        InputPartition const& input_partition = array.partitions()[p];

        output_partitions[p] =
            hpx::get_colocation_id(input_partition.get_id()).then(
                hpx::util::unwrapping(
                    [=](
                        hpx::id_type const locality_id)
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


/*!
    @overload
*/
template<
    typename Element,
    std::size_t rank,
    template<typename, std::size_t> typename Array>
PartitionedArrayT<Array<Element, rank>, bool> less(
    hpx::shared_future<Element> const& scalar,
    Array<Element, rank> const& array)
{
    using InputArray = Array<Element, rank>;
    using InputScalar = hpx::shared_future<Element>;
    using InputPartition = PartitionT<InputArray>;

    using OutputArray = PartitionedArrayT<InputArray, bool>;
    using OutputPartitions = PartitionsT<OutputArray>;

    LessPartitionAction<InputScalar, InputPartition> action;
    OutputPartitions output_partitions{shape_in_partitions(array)};

    for(std::size_t p = 0; p < nr_partitions(array); ++p) {

        InputPartition const& input_partition = array.partitions()[p];

        output_partitions[p] =
            hpx::get_colocation_id(input_partition.get_id()).then(
                hpx::util::unwrapping(
                    [=](
                        hpx::id_type const locality_id)
                    {
                        return hpx::dataflow(
                            hpx::launch::async,
                            action,
                            locality_id,
                            scalar,
                            input_partition);
                    }
                )
            );
    }

    return OutputArray{shape(array), std::move(output_partitions)};


}

}  // namespace lue
