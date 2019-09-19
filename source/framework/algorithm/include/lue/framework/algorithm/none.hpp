#pragma once
#include "lue/framework/core/type_traits.hpp"
#include <hpx/include/lcos.hpp>
#include <type_traits>


namespace lue {
namespace detail {

template<
    typename Partition,
    typename Element>
Partition none_partition(
    Partition const& partition)
{
    assert(
        hpx::get_colocation_id(partition.get_id()).get() ==
        hpx::find_here());

    using InputData = DataT<Partition>;

    using OutputData = DataT<Partition>;

    using Shape = ShapeT<Partition>;

    hpx::shared_future<InputData> partition_data =
        partition.data(CopyMode::share);

    // Aggregate nD array partition to nD array partition containing a
    // single value
    Shape shape;
    std::fill(shape.begin(), shape.end(), 1);

    hpx::future<OutputData> none = partition_data.then(
        hpx::util::unwrapping(
            [shape](
                InputData const& partition_data)
            {
                // TODO Update for case where Element is not bool
                Element result = std::find(
                    partition_data.begin(), partition_data.end(),
                    Element{1}) == partition_data.end();

                return OutputData{shape, result};
            }
        )
    );

    // Once the result has been calculated, create a component containing
    // the result, on the same locality as the summed partition
    return hpx::when_all(hpx::get_colocation_id(partition.get_id()), none)
        .then(
            hpx::util::unwrapping(
                [](
                    auto&& futures)
                {
                    auto const locality_id = hpx::util::get<0>(futures).get();
                    auto&& data = hpx::util::get<1>(futures).get();

                    return Partition{locality_id, std::move(data)};
                }
            )
        );
}

}  // namespace detail


template<
    typename Partition,
    typename Element>
struct NonePartitionAction:
    hpx::actions::make_action<
        decltype(&detail::none_partition<Partition, Element>),
        &detail::none_partition<Partition, Element>,
        NonePartitionAction<Partition, Element>>
{};


/*!
    @brief      Return whether a partitioned array contains only elements
                that evaluate to true
    @tparam     Element Type of elements in the arrays
    @tparam     rank Rank of the input arrays
    @tparam     Array Class template of the type of the arrays
    @param      array Partitioned array
    @return     Future to a value that evaluates to true or false
*/
template<
    typename Element,
    std::size_t rank,
    template<typename, std::size_t> typename Array>
hpx::future<Element> none(
    Array<Element, rank> const& array)
{
    static_assert(std::is_convertible_v<Element, bool>);

    using InputArray = Array<Element, rank>;
    using InputPartition = PartitionT<InputArray>;

    using OutputPartitions = PartitionsT<InputArray, Element>;

    OutputPartitions output_partitions{shape_in_partitions(array)};
    NonePartitionAction<InputPartition, Element> action;

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

    // The partition results are being determined on their respective
    // localities. Attach a continuation that aggregates the results
    // once the partition results are ready. This continuation runs on
    // our locality.
    return hpx::when_all(
            output_partitions.begin(), output_partitions.end()).then(
        hpx::util::unwrapping(
            [](auto const& partitions) {

                Element result{0};

                for(auto const& partition: partitions) {

                    auto const data = partition.data(CopyMode::copy).get();
                    assert(data.size() == 1);
                    result = data[0];

                    if(result) {
                        // Short-circuit
                        break;
                    }

                }

                return result;
            }
        )
    );
}

}  // namespace lue
