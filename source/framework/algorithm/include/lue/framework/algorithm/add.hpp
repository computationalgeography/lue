#pragma once
#include "lue/framework/core/component/array_partition.hpp"
#include <hpx/include/lcos.hpp>


namespace lue {
namespace detail {

template<
    typename Partition>
Partition add_partition(
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

    using Data = DataT<Partition>;
    using Element = ElementT<Partition>;

    // Asynchronously retrieve the partition data from the array partition
    // components
    hpx::shared_future<Data> partition_data1 = partition1.data();
    hpx::shared_future<Data> partition_data2 = partition2.data();

    // Once the data has arrived, sum the values
    hpx::future<Data> addition = hpx::dataflow(
        hpx::launch::async,
        hpx::util::unwrapping([](
            Data const& partition_data1,
            Data const& partition_data2)
        {
            assert(partition_data1.shape() == partition_data2.shape());

            Data result{partition_data1.shape()};

            std::transform(
                partition_data1.begin(),
                partition_data1.end(),
                partition_data2.begin(),
                result.begin(), std::plus<Element>{});

            return result;
        }),
        partition_data1,
        partition_data2
    );

    // Once the addition has been calculated, create a new component
    // containing the result, on the same locality as the first partition
    // passed in
    return addition.then(
        // TODO Pass in ref to partition?
        hpx::util::unwrapping([partition1](
            Data&& addition_data)
        {
            return Partition(partition1.get_id(), addition_data);
        })
    );
}


template<
    typename Element,
    std::size_t rank>
struct AddPartitionAction
{
};

}  // namespace detail
}  // namespace lue


#define LUE_ADD_PLAIN_ACTION(                                         \
    Element,                                                          \
    rank)                                                             \
                                                                      \
namespace lue {                                                       \
namespace detail {                                                    \
                                                                      \
decltype(auto) add_partition_##Element##_##rank(                      \
    ArrayPartition<Element, rank> const& partition1,                  \
    ArrayPartition<Element, rank> const& partition2)                  \
{                                                                     \
    using Partition = ArrayPartition<Element, rank>;                  \
                                                                      \
    return add_partition<Partition>(partition1, partition2);          \
}                                                                     \
                                                                      \
}                                                                     \
}                                                                     \
                                                                      \
                                                                      \
HPX_PLAIN_ACTION(                                                     \
    lue::detail::add_partition_##Element##_##rank,                    \
    AddPartitionAction_##Element##_##rank)                            \
                                                                      \
                                                                      \
namespace lue {                                                       \
namespace detail {                                                    \
                                                                      \
template<>                                                            \
class AddPartitionAction<Element, rank>                               \
{                                                                     \
                                                                      \
public:                                                               \
                                                                      \
    using Type = AddPartitionAction_##Element##_##rank;               \
                                                                      \
};                                                                    \
                                                                      \
}                                                                     \
}


#define LUE_ADD_PLAIN_ACTIONS(    \
    Element)                      \
                                  \
LUE_ADD_PLAIN_ACTION(Element, 1)  \
LUE_ADD_PLAIN_ACTION(Element, 2)


LUE_ADD_PLAIN_ACTIONS(/* std:: */ int32_t)
// LUE_ADD_PLAIN_ACTIONS(/* std:: */ int64_t)
// LUE_ADD_PLAIN_ACTIONS(float)
LUE_ADD_PLAIN_ACTIONS(double)


#undef LUE_ADD_PLAIN_ACTION
#undef LUE_ADD_PLAIN_ACTIONS


namespace lue {

template<
    typename Element,
    std::size_t rank>
using AddPartitionActionType =
    typename detail::AddPartitionAction<Element, rank>::Type;


template<
    typename Array>
Array add(
    Array const& array1,
    Array const& array2)
{
    assert(nr_partitions(array1) == nr_partitions(array2));

    using Partitions = PartitionsT<Array>;
    using Partition = PartitionT<Array>;
    using Element = ElementT<Array>;

    AddPartitionActionType<Element, rank<Array>> add_partition_action;

    Partitions partitions{shape_in_partitions(array1)};

    // Attach a continuation to each pair of input partitions that adds
    // all elements in those partitions and assigns the result to the
    // output partition
    for(std::size_t p = 0; p < nr_partitions(array1); ++p) {

        Partition const& input_partition1 = array1.partitions()[p];
        Partition const& input_partition2 = array2.partitions()[p];
        Partition& output_partition = partitions[p];

        output_partition = hpx::dataflow(
            hpx::launch::async,
            add_partition_action,
            hpx::get_colocation_id(
                hpx::launch::sync, input_partition1.get_id()),
            input_partition1,
            input_partition2);
    }

    return Array{shape(array1), std::move(partitions)};
}

}  // namespace lue
