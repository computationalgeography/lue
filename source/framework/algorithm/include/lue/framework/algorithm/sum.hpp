#pragma once
#include "lue/framework/core/type_traits.hpp"
#include <hpx/include/lcos.hpp>


namespace lue {
namespace detail {

/*!
    @brief      Return the sum of values in an array partition
    @tparam     Partition Client class of partition component
    @tparam     OutputElement Element type for storing the sum
    @param      partition Client of array partition component
    @return     Client of an array partition component with a sum in it
*/
template<
    typename Partition,
    typename OutputElement>  /// =ElementT<Partition>>
PartitionT<Partition, OutputElement> sum_partition(
    Partition const& partition)
{
    assert(
        hpx::get_colocation_id(partition.get_id()).get() ==
        hpx::find_here());

    using InputData = DataT<Partition>;

    using OutputPartition = PartitionT<Partition, OutputElement>;
    using OutputData = DataT<OutputPartition>;

    using Shape = ShapeT<Partition>;

    hpx::shared_future<InputData> partition_data =
        partition.data(CopyMode::share);

    // Aggregate nD array partition to nD array partition containing a
    // single value
    Shape shape;
    std::fill(shape.begin(), shape.end(), 1);

    hpx::future<OutputData> sum = partition_data.then(
        hpx::util::unwrapping(
            [shape](
                InputData const& partition_data)
            {
                OutputElement result = std::accumulate(
                    partition_data.begin(), partition_data.end(), 0);

                return OutputData{shape, result};
            }
        )
    );

    // Once the sum has been calculated, create a component containing
    // the result, on the same locality as the summed partition
    return hpx::when_all(hpx::get_colocation_id(partition.get_id()), sum)
        .then(
            hpx::util::unwrapping(
                [](
                    auto&& futures)
                {
                    auto const locality_id = hpx::util::get<0>(futures).get();
                    auto&& data = hpx::util::get<1>(futures).get();

                    return Partition{locality_id, data, std::string{"meh"}};
                }
            )
        );
}

}  // namespace detail


template<
    typename Partition,
    typename OutputElement>
struct SumPartitionAction:
    hpx::actions::make_action<
        decltype(&detail::sum_partition<Partition, OutputElement>),
        &detail::sum_partition<Partition, OutputElement>,
        SumPartitionAction<Partition, OutputElement>>
{};


/*!
    @brief      Return the result of summing the elements in a partitioned
                array
    @tparam     InputElement Type of elements in the input array
    @tparam     OutputElement Type of result value
    @tparam     rank Rank of the input array
    @tparam     Array Class template of the type of the array
    @param      array Partitioned array
    @return     Future that becomes ready once the algorithm has finished
*/
template<
    typename InputElement,
    typename OutputElement=InputElement,
    std::size_t rank,
    template<typename, std::size_t> typename Array>
hpx::future<OutputElement> sum(
    Array<InputElement, rank> const& array)
{
    using InputArray = Array<InputElement, rank>;
    using InputPartition = PartitionT<InputArray>;

    using OutputPartitions = PartitionsT<InputArray, OutputElement>;

    OutputPartitions output_partitions{shape_in_partitions(array)};
    SumPartitionAction<InputPartition, OutputElement> action;

    for(std::size_t p = 0; p < nr_partitions(array); ++p) {

        InputPartition const& partition = array.partitions()[p];

        output_partitions[p] =
            hpx::get_colocation_id(partition.get_id()).then(
                hpx::util::unwrapping(
                    [=](
                        hpx::id_type const locality_id)
                    {
                        return hpx::dataflow(
                            hpx::launch::async,
                            action,
                            locality_id,
                            partition);
                    }
                )
            );

    }

    // The partition sums are being determined on their respective
    // localities. Attach a continuation that sums the sums once the
    // partition sums are ready. This continuation runs on our locality.
    return hpx::when_all(
            output_partitions.begin(), output_partitions.end()).then(
        hpx::util::unwrapping(
            [](auto const& partitions) {

                OutputElement result{};

                for(auto const& partition: partitions) {

                    auto const data = partition.data(CopyMode::copy).get();
                    assert(data.size() == 1);

                    result += data[0];

                }

                return result;
            }
        )
    );
}

}  // namespace lue
