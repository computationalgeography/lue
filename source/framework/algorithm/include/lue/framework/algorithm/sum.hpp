#pragma once
#include "lue/framework/core/array_partition_data.hpp"
#include "lue/framework/core/component/array_partition.hpp"
#include "lue/framework/define.hpp"
#include <hpx/hpx.hpp>


namespace lue {
namespace detail {

/*!
    @brief      Return the sum of values in an array partition
    @tparam     Partition Client class of partition component
    @tparam     ResultElement Element type for storing the sum
    @param      partition Client of array partition component
    @return     Client of an array partition component with a sum in it
*/
template<
    typename Partition,
    typename ResultElement=
        typename PartitionedArrayTypeTraits<Partition>::ElementType>
typename ArrayPartitionTypeTraits<Partition>::
        template PartitionTemplate<ResultElement> sum_partition(
    Partition const& partition)
{
    // Assert the locality of the partition is the same as the locality
    // this code runs on
    assert(
        hpx::get_colocation_id(partition.get_id()).get() ==
        hpx::find_here());

    using Data = typename ArrayPartitionTypeTraits<Partition>::DataType;
    using ResultPartition =
        typename ArrayPartitionTypeTraits<Partition>::
            template PartitionTemplate<ResultElement>;
    using ResultData =
        typename ArrayPartitionTypeTraits<ResultPartition>::DataType;
    using Shape = typename ArrayPartitionTypeTraits<Partition>::ShapeType;

    // Retrieve the partition data
    hpx::shared_future<Data> partition_data = partition.data();

    // Aggregate nD array partition to nD array partition
    // containing a single value
    Shape shape;
    std::fill(shape.begin(), shape.end(), 1);

    // Once the data has arrived, sum the values
    hpx::future<ResultData> sum = partition_data.then(
        hpx::util::unwrapping(
            [shape](Data const& partition_data)
            {
                ResultElement result = std::accumulate(
                    partition_data.data(),
                    partition_data.data() + partition_data.size(), 0);

                return ResultData{shape, result};
            }
        )
    );

    // Once the sum has been calculated, create a component containing
    // the result, on the same locality as the summed partition
    return sum.then(
        hpx::util::unwrapping(
            [partition](ResultData&& sum_data)
            {
                assert(sum_data.size() == 1);

                return ResultPartition(partition.get_id(), sum_data);
            }
        )
    );
}


#define LUE_SUM_PARTITION_SPECIALIZATION(                          \
    T1, t1,                                                        \
    ResultElement, result_element,                                 \
    rank)                                                          \
decltype(auto) sum_partition_##t1##_##result_element##_##rank(     \
        client::ArrayPartition<                                    \
            T1,                                                    \
            ArrayPartitionData<T1, rank>> const& partition)        \
{                                                                  \
    using Partition =                                              \
        client::ArrayPartition<T1, ArrayPartitionData<T1, rank>>;  \
                                                                   \
    return sum_partition<Partition, ResultElement>(partition);     \
}

LUE_SUM_PARTITION_SPECIALIZATION(
    std::int32_t, int32_t,
    std::int32_t, int32_t,
    1)
LUE_SUM_PARTITION_SPECIALIZATION(
    std::int32_t, int32_t,
    std::int32_t, int32_t,
    2)

LUE_SUM_PARTITION_SPECIALIZATION(
    std::int32_t, int32_t,
    std::int64_t, int64_t,
    1)
LUE_SUM_PARTITION_SPECIALIZATION(
    std::int32_t, int32_t,
    std::int64_t, int64_t,
    2)

#undef LUE_SUM_PARTITION_SPECIALIZATION

}  // namespace detail
}  // namespace lue


#define LUE_SUM_PARTITION_PLAIN_ACTION(        \
    T1, t1,                                    \
    ResultElement, result_element,             \
    rank)                                      \
HPX_PLAIN_ACTION(                              \
    lue::detail::sum_partition_##t1##_##result_element##_##rank,  \
    SumPartitionAction_##t1##_##result_element##_##rank);

LUE_SUM_PARTITION_PLAIN_ACTION(
    std::int32_t, int32_t,
    std::int32_t, int32_t,
    1)
LUE_SUM_PARTITION_PLAIN_ACTION(
    std::int32_t, int32_t,
    std::int32_t, int32_t,
    2)

LUE_SUM_PARTITION_PLAIN_ACTION(
    std::int32_t, int32_t,
    std::int64_t, int64_t,
    1)
LUE_SUM_PARTITION_PLAIN_ACTION(
    std::int32_t, int32_t,
    std::int64_t, int64_t,
    2)

#undef LUE_SUM_PARTITION_PLAIN_ACTION


#define LUE_SUM_PARTITION_PLAIN_ACTION_PRIMARY_TEMPLATE()  \
template<                                                  \
    typename T1,                                           \
    typename ResultElement,                                \
    std::size_t rank>                                      \
struct SumPartitionAction                                  \
{                                                          \
};


#define LUE_SUM_PARTITION_PLAIN_ACTION_TEMPLATE(    \
    T1, t1,                                         \
    ResultElement, result_element,                  \
    rank)                                           \
template<>                                          \
struct SumPartitionAction<T1, ResultElement, rank>  \
{                                                   \
    using Type = SumPartitionAction_##t1##_##result_element##_##rank;  \
};


namespace lue {
namespace detail {

LUE_SUM_PARTITION_PLAIN_ACTION_PRIMARY_TEMPLATE()

LUE_SUM_PARTITION_PLAIN_ACTION_TEMPLATE(
    std::int32_t, int32_t,
    std::int32_t, int32_t,
    1)
LUE_SUM_PARTITION_PLAIN_ACTION_TEMPLATE(
    std::int32_t, int32_t,
    std::int32_t, int32_t,
    2)

LUE_SUM_PARTITION_PLAIN_ACTION_TEMPLATE(
    std::int32_t, int32_t,
    std::int64_t, int64_t,
    1)
LUE_SUM_PARTITION_PLAIN_ACTION_TEMPLATE(
    std::int32_t, int32_t,
    std::int64_t, int64_t,
    2)

#undef LUE_SUM_PARTITION_PLAIN_ACTION_PRIMARY_TEMPLATE
#undef LUE_SUM_PARTITION_PLAIN_ACTION_TEMPLATE

}  // namespace detail


template<
    typename Element,
    typename ResultElement,
    std::size_t rank>
using SumPartitionActionType =
    typename detail::SumPartitionAction<Element, ResultElement, rank>::Type;


template<
    typename Array,
    typename ResultElement=
        typename PartitionedArrayTypeTraits<Array>::ElementType>
hpx::future<ResultElement> sum(
    Array const& array)
{
    // TODO Calling lue::rank(array) triggers a compiler error
    static std::size_t const rank = Array::rank;

    using ArrayTraits = PartitionedArrayTypeTraits<Array>;
    using Partitions = typename ArrayTraits::PartitionsType;
    using Partition = typename ArrayTraits::PartitionType;
    using Element = typename ArrayTraits::ElementType;
    using SumPartitionAction =
        SumPartitionActionType<Element, ResultElement, rank>;

    // TODO Make this work
    // using SumsPartitions =
    //     typename ArrayTraits:: template PartitionsTemplate<ResultElement>;

    using SumPartition =
        client::ArrayPartition<
            ResultElement,
            ArrayPartitionData<ResultElement, rank>
        >;
    using SumsPartitions = std::vector<SumPartition>;

    SumsPartitions sums_partitions{nr_elements(shape_in_partitions(array))};
    SumPartitionAction sum_partition_action;

    // Attach a continuation to each array partition that calculates
    // the sum of the values in that partition. These continuations run
    // on the same localities as where the partitions themselves are
    // located.
    for(std::size_t p = 0; p < nr_partitions(array); ++p) {

        // TODO How does this work? What is being passed to the action?
        //     Partition is a component client. Is its ID passed and
        //     wrapped in a new client instance which is passed into the action?
        Partition const& partition = array.partitions()[p];

        sums_partitions[p] = hpx::dataflow(
            hpx::launch::async,
            sum_partition_action,
            // TODO Is this efficient?
            hpx::get_colocation_id(hpx::launch::sync, partition.get_id()),
            // TODO This partition client instance is copied here isn't it?
            //     Does that make sense?
            //     Partition clients must be like shared futures. Are they?
            partition
        );
    }

    // The partition sums are being determined on their respective
    // localities. Attach a continuation that sums the sums once the
    // partition sums are ready. This continuation runs on our own
    // locality.
    return hpx::dataflow(
        hpx::launch::async,
        hpx::util::unwrapping(

            // IDs of the partitions containing the partition sums
            [](auto&& sums_partitions_ids) {

                // Sum all partition sums and return the result
                ResultElement result{};

                for(auto const& partition_id: sums_partitions_ids) {

                    // Create partition component client and obtain the
                    // single value representing the sum of the partition
                    SumPartition partition{partition_id};
                    auto data = partition.data().get();
                    assert(data.size() == 1);

                    result += data[0];

                }

                return result;

            }
        ),
        std::move(sums_partitions)
    );
}

}  // namespace lue
