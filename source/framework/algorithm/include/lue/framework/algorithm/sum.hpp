#pragma once
#include "lue/framework/core/component/array_partition.hpp"
#include <hpx/dataflow.hpp>


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
                    partition_data.begin(), partition_data.end(), 0);

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


template<
    typename Element,
    typename ResultElement,
    std::size_t rank>
struct SumPartitionAction
{
};

}  // namespace detail
}  // namespace lue


#define LUE_SUM_PLAIN_ACTION(                                         \
    Element,                                                          \
    ResultElement,                                                    \
    rank)                                                             \
                                                                      \
namespace lue {                                                       \
namespace detail {                                                    \
                                                                      \
decltype(auto) sum_partition_##Element##_##ResultElement##_##rank(    \
    ArrayPartition<Element, rank> const& partition)                   \
{                                                                     \
    using Partition = ArrayPartition<Element, rank>;                  \
                                                                      \
    return sum_partition<Partition, ResultElement>(partition);        \
}                                                                     \
                                                                      \
}                                                                     \
}                                                                     \
                                                                      \
                                                                      \
HPX_PLAIN_ACTION(                                                     \
    lue::detail::sum_partition_##Element##_##ResultElement##_##rank,  \
    SumPartitionAction_##Element##_##ResultElement##_##rank)          \
                                                                      \
                                                                      \
namespace lue {                                                       \
namespace detail {                                                    \
                                                                      \
template<>                                                            \
class SumPartitionAction<Element, ResultElement, rank>                \
{                                                                     \
                                                                      \
public:                                                               \
                                                                      \
    using Type =                                                      \
        SumPartitionAction_##Element##_##ResultElement##_##rank;      \
                                                                      \
};                                                                    \
                                                                      \
}                                                                     \
}


#define LUE_SUM_PLAIN_ACTIONS(                   \
    Element,                                     \
    ResultElement)                               \
                                                 \
LUE_SUM_PLAIN_ACTION(Element, ResultElement, 1)  \
LUE_SUM_PLAIN_ACTION(Element, ResultElement, 2)


LUE_SUM_PLAIN_ACTIONS(/* std:: */ int32_t, /* std:: */ int32_t)
// LUE_SUM_PLAIN_ACTIONS(/* std:: */ int64_t, /* std:: */ int64_t)
// LUE_SUM_PLAIN_ACTIONS(/* std:: */ int32_t, /* std:: */ int64_t)
// LUE_SUM_PLAIN_ACTIONS(float, float)
LUE_SUM_PLAIN_ACTIONS(double, double)
// LUE_SUM_PLAIN_ACTIONS(float, double)


#undef LUE_SUM_PLAIN_ACTION
#undef LUE_SUM_PLAIN_ACTIONS


namespace lue {

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
    using ArrayTraits = PartitionedArrayTypeTraits<Array>;
    using Partition = typename ArrayTraits::PartitionType;
    using Element = typename ArrayTraits::ElementType;

    using SumsPartitions =
        typename ArrayTraits:: template PartitionsTemplate<ResultElement>;
    using SumPartition =
        typename ArrayPartitionTypeTraits<Partition>::
            template PartitionTemplate<ResultElement>;

    using SumPartitionAction =
        SumPartitionActionType<Element, ResultElement, rank(array)>;

    SumsPartitions sums_partitions{shape_in_partitions(array)};
    SumPartitionAction sum_partition_action;

    // Attach a continuation to each array partition, that calculates
    // the sum of the values in that partition. These continuations run
    // on the same localities as where the partitions themselves are
    // located.
    for(std::size_t p = 0; p < nr_partitions(array); ++p) {

        Partition const& partition = array.partitions()[p];

        sums_partitions[p] = hpx::dataflow(
            hpx::launch::async,
            sum_partition_action,
            // TODO Is this efficient?
            hpx::get_colocation_id(hpx::launch::sync, partition.get_id()),
            partition
        );

    }


    // The partition sums are being determined on their respective
    // localities. Attach a continuation that sums the sums once the
    // partition sums are ready. This continuation runs on our own
    // locality.
    return hpx::when_all(sums_partitions.begin(), sums_partitions.end()).then(
        [](auto&& f) {
            auto const sums_partitions = f.get();

            // Sum all partition sums and return the result
            ResultElement result{};

            for(auto const& sums_partition: sums_partitions) {

                auto const data = sums_partition.data().get();
                assert(data.size() == 1);

                result += data[0];

            }

            return result;
        }
    );
}

}  // namespace lue
