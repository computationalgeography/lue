#pragma once
#include "lue/framework/core/component/array_partition.hpp"
#include <hpx/include/lcos.hpp>


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
    typename ResultElement>  /// =ElementT<Partition>>
PartitionT<Partition, ResultElement> sum_partition(
    Partition const& partition)
{
    assert(
        hpx::get_colocation_id(partition.get_id()).get() ==
        hpx::find_here());

    using Data = DataT<Partition>;
    using ResultPartition = PartitionT<Partition, ResultElement>;
    using ResultData = DataT<ResultPartition>;
    using Shape = ShapeT<Partition>;

    // Retrieve the partition data
    hpx::shared_future<Data> partition_data = partition.data(CopyMode::share);

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

}  // namespace detail


template<
    typename Partition,
    typename ResultElement>
struct SumPartitionAction:
    hpx::actions::make_action<
        decltype(&detail::sum_partition<Partition, ResultElement>),
        &detail::sum_partition<Partition, ResultElement>,
        SumPartitionAction<Partition, ResultElement>>
{};


template<
    typename Array,
    typename ResultElement=ElementT<Array>>
hpx::future<ResultElement> sum(
    Array const& array)
{
    using Partition = PartitionT<Array>;
    using Element = ElementT<Array>;

    using SumsPartitions = PartitionsT<Array, ResultElement>;
    using SumPartition = PartitionT<Array, ResultElement>;

    SumsPartitions sums_partitions{shape_in_partitions(array)};
    SumPartitionAction<Partition, ResultElement> sum_partition_action;

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

                auto const data = sums_partition.data(CopyMode::copy).get();
                assert(data.size() == 1);

                result += data[0];

            }

            return result;
        }
    );
}

}  // namespace lue
