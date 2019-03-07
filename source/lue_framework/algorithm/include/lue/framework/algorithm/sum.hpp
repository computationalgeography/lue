#pragma once
#include "lue/framework/core/array_partition_data.hpp"
#include "lue/framework/core/component/array_partition.hpp"
#include "lue/framework/define.hpp"
#include <hpx/hpx.hpp>
// #include <gsl/gsl>


namespace lue {

// -----------------------------------------------------------------------------
// core/array_type_traits.hpp
// Default array type traits
template<
    typename Array>
class ArrayTypeTraits
{
};


// Default array partition type traits
template<
    typename Array>
class ArrayPartitionTypeTraits
{
};


// -----------------------------------------------------------------------------
// core/array.hpp
// Default array funtion templates
template<
    typename Array>
std::size_t rank(Array const& array);


template<
    typename Array,
    typename Shape>
Shape shape_in_partitions(Array const& array);


template<
    typename Array>
std::size_t nr_partitions(Array const& array);

}  // namespace lue


#include "lue/framework/core/component/partitioned_array.hpp"


namespace lue {

// -----------------------------------------------------------------------------
// core/partitioned_array_type_traits.hpp
template<
    typename ElementType_,
    typename Data>
class ArrayTypeTraits<PartitionedArray<ElementType_, Data>>
{

public:

    using Array = PartitionedArray<ElementType_, Data>;
    using ElementType = typename Array::ElementType;
    using ShapeType = typename Array::ShapeType;
    using PartitionsType = typename Array::Partitions;
    using PartitionType = typename Array::Partition;

};


template<
    typename ElementType,
    typename Data>
constexpr std::size_t rank(
    PartitionedArray<ElementType, Data> const& /* array */) noexcept
{
    // For this array type, rank is a compile time constant, so the
    // instance is not used
    return PartitionedArray<ElementType, Data>::rank;
}


template<
    typename ElementType,
    typename Data>
typename ArrayTypeTraits<PartitionedArray<ElementType, Data>>::ShapeType
        shape_in_partitions(
    PartitionedArray<ElementType, Data> const& array)
{
    return array.partitions().shape();
}


template<
    typename ElementType,
    typename Data>
std::size_t nr_partitions(
    PartitionedArray<ElementType, Data> const& array)
{
    return array.nr_partitions();
}


// -----------------------------------------------------------------------------

template<
    typename ElementType_,
    typename Data>
class ArrayPartitionTypeTraits<client::ArrayPartition<ElementType_, Data>>
{

private:

    using Partition = client::ArrayPartition<ElementType_, Data>;

public:

    using DataType = typename Partition::DataType;
    using ShapeType = typename Partition::ShapeType;

    template<
        typename ElementType>
    using PartitionType = client::ArrayPartition<ElementType, Data>;

};

}  // namespace lue


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
    typename ResultElement=typename ArrayTypeTraits<Partition>::ElementType>
typename ArrayPartitionTypeTraits<Partition>::
        template PartitionType<ResultElement> sum_partition(
    Partition const& partition)
{
    // TODO Assert the current locality equals the one of the server
    // auto hpx::naming::get_locality_id_from_id(r.first);

    using Data = typename ArrayPartitionTypeTraits<Partition>::DataType;
    using ResultPartition = typename
        ArrayPartitionTypeTraits<Partition>::template PartitionType<ResultElement>;
    using ResultData = typename
        ArrayPartitionTypeTraits<Partition>::DataType;
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
        })
    );

    // Once the sum has been calculated, create a component containing
    // the result, on the same locality as the summed partition
    return sum.then(
        hpx::util::unwrapping(
            [partition](ResultData&& sum_data)
        {
            assert(sum_data.size() == 1);

            return ResultPartition(partition.get_id(), sum_data);
        })
    );
}


// TODO Support passing return element type
#define LUE_SUM_PARTITION_SPECIALIZATION(                          \
    T1, t1,                                                        \
    rank)                                                          \
decltype(auto) sum_partition_##t1##_##rank(                        \
        client::ArrayPartition<                                    \
            T1,                                                    \
            ArrayPartitionData<T1, rank>> const& partition)        \
{                                                                  \
    using Partition =                                              \
        client::ArrayPartition<T1, ArrayPartitionData<T1, rank>>;  \
    using ResultElement = T1;                                      \
                                                                   \
    return sum_partition<Partition, ResultElement>(partition);     \
}

LUE_SUM_PARTITION_SPECIALIZATION(std::int32_t, int32_t, 1)
LUE_SUM_PARTITION_SPECIALIZATION(std::int32_t, int32_t, 2)

}  // namespace detail
}  // namespace lue


#define LUE_SUM_PARTITION_PLAIN_ACTION(        \
    T1, t1,                                    \
    rank)                                      \
HPX_PLAIN_ACTION(                              \
    lue::detail::sum_partition_##t1##_##rank,  \
    SumPartitionAction_##t1##_##rank);

LUE_SUM_PARTITION_PLAIN_ACTION(std::int32_t, int32_t, 1)
LUE_SUM_PARTITION_PLAIN_ACTION(std::int32_t, int32_t, 2)

#undef LUE_SUM_PARTITION_PLAIN_ACTION


#define LUE_SUM_PARTITION_PLAIN_ACTION_PRIMARY_TEMPLATE()  \
template<                                                  \
    typename T1,                                           \
    std::size_t rank>                                      \
struct SumPartitionAction                                  \
{                                                          \
};


#define LUE_SUM_PARTITION_PLAIN_ACTION_TEMPLATE(    \
    T1, t1,                                         \
    rank)                                           \
template<>                                          \
struct SumPartitionAction<T1, rank>                 \
{                                                   \
    using Type = SumPartitionAction_##t1##_##rank;  \
};


namespace lue {
namespace detail {

LUE_SUM_PARTITION_PLAIN_ACTION_PRIMARY_TEMPLATE()
LUE_SUM_PARTITION_PLAIN_ACTION_TEMPLATE(std::int32_t, int32_t, 1)
LUE_SUM_PARTITION_PLAIN_ACTION_TEMPLATE(std::int32_t, int32_t, 2)

#undef LUE_SUM_PARTITION_PLAIN_ACTION_PRIMARY_TEMPLATE
#undef LUE_SUM_PARTITION_PLAIN_ACTION_TEMPLATE

}  // namespace detail


template<
    typename Element,
    std::size_t rank>
using SumPartitionActionType =
    typename detail::SumPartitionAction<Element, rank>::Type;


template<
    typename Array,
    typename Return=typename ArrayTypeTraits<Array>::ElementType>
hpx::future<Return> sum(
    Array const& array)
{
    using ArrayTraits = ArrayTypeTraits<Array>;
    using Partitions = typename ArrayTraits::PartitionsType;
    using Partition = typename ArrayTraits::PartitionType;
    using Element = typename ArrayTraits::ElementType;
    using SumPartitionAction = SumPartitionActionType<Element, rank(array)>;

    Partitions sums_partitions{shape_in_partitions(array)};
    SumPartitionAction sum_partition_action;

    for(std::size_t p = 0; p < nr_partitions(array); ++p) {

        sums_partitions[p] = hpx::dataflow(
            hpx::launch::async,
            sum_partition_action,
            hpx::find_here(),  // TODO: Use locality of input partition
            array.partitions()[p]);

    }


    // TODO Why does this SIGABRT?
    // return hpx::when_all(
    //         gsl::span<Partition>(
    //             sums_partitions.data(), sums_partitions.size()))
    return hpx::when_all_n(sums_partitions.data(), sums_partitions.size())
        .then(
            [](auto&& sums_partitions)
            {
                // Sum all partition sums and return the result
                auto partitions = sums_partitions.get();

                Return result{};

                for(auto const& partition_id: partitions) {

                    Partition partition{partition_id.get()};
                    auto data = partition.data().get();
                    assert(data.size() == 1);

                    result += data[0];

                }

                return result;
            }
        );
}

}  // namespace lue
