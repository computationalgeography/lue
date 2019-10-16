#pragma once
#include "lue/framework/core/type_traits.hpp"
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
                            return lhs == rhs;
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
                            return element == scalar;
                        });

                    return OutputPartition{locality_id, std::move(result)};
                }

            ),
            hpx::get_colocation_id(partition.get_id()),
            partition.data(CopyMode::share));
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
PartitionedArrayT<Array<bool, rank>, bool> equal_to(
    Array<Element, rank> const& array1,
    Array<Element, rank> const& array2)
{
    assert(nr_partitions(array1) == nr_partitions(array2));

    using InputArray = Array<Element, rank>;
    using InputPartition = PartitionT<InputArray>;

    using OutputArray = PartitionedArrayT<InputArray, bool>;
    using OutputPartitions = PartitionsT<OutputArray>;

    EqualToPartitionAction<InputPartition, InputPartition> action;
    OutputPartitions output_partitions{shape_in_partitions(array1)};

    for(std::size_t p = 0; p < nr_partitions(array1); ++p) {

        output_partitions[p] = hpx::dataflow(
            hpx::launch::async,
            hpx::util::unwrapping(

                [action](
                    hpx::id_type const component1_id,
                    hpx::id_type const component2_id)
                {
                    return action(
                        hpx::get_colocation_id(
                            hpx::launch::sync, component1_id),
                        InputPartition{component1_id},
                        InputPartition{component2_id});
                }

            ),
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

    EqualToPartitionAction<InputPartition, InputScalar> action;
    OutputPartitions output_partitions{shape_in_partitions(array)};

    for(std::size_t p = 0; p < nr_partitions(array); ++p) {

        output_partitions[p] = hpx::dataflow(
            hpx::launch::async,
            hpx::util::unwrapping(

                [action](
                    hpx::id_type const component_id,
                    Element const scalar)
                {
                    return action(
                        hpx::get_colocation_id(
                            hpx::launch::sync, component_id),
                        InputPartition{component_id},
                        scalar);
                }

            ),
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
    std::size_t rank,
    template<typename, std::size_t> typename Array>
PartitionedArrayT<Array<Element, rank>, bool> equal_to(
    hpx::shared_future<Element> const& scalar,
    Array<Element, rank> const& array)
{
    return equal_to(array, scalar);
}

}  // namespace lue
