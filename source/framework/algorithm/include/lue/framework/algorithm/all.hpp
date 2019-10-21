#pragma once
#include "lue/framework/core/type_traits.hpp"
#include <hpx/include/lcos.hpp>
#include <type_traits>


namespace lue {
namespace detail {

template<
    typename Partition,
    typename Element>
Partition all_partition(
    Partition const& partition)
{
    assert(
        hpx::get_colocation_id(partition.get_id()).get() ==
        hpx::find_here());

    using InputData = DataT<Partition>;

    using OutputData = DataT<Partition>;

    using Shape = ShapeT<Partition>;

    // Aggregate nD array partition to nD array partition containing a
    // single value
    Shape shape;
    std::fill(shape.begin(), shape.end(), 1);

    return hpx::dataflow(
        hpx::launch::async,
        hpx::util::unwrapping(

            [shape](
                hpx::id_type const locality_id,
                InputData&& partition_data)
            {
                // TODO Update for case where Element is not bool
                // If one of the elements evaluates to false, then the result
                // is false
                Element result = !(std::find(
                    partition_data.begin(), partition_data.end(),
                    Element{0}) != partition_data.end());

                return Partition{locality_id, OutputData{shape, result}};
            }

        ),
        hpx::get_colocation_id(partition.get_id()),
        partition.data(CopyMode::share));
}

}  // namespace detail


template<
    typename Partition,
    typename Element>
struct AllPartitionAction:
    hpx::actions::make_action<
        decltype(&detail::all_partition<Partition, Element>),
        &detail::all_partition<Partition, Element>,
        AllPartitionAction<Partition, Element>>
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
hpx::future<Element> all(
    Array<Element, rank>const& array)
{
    static_assert(std::is_convertible_v<Element, bool>);

    using InputArray = Array<Element, rank>;
    using InputPartition = PartitionT<InputArray>;

    using OutputPartitions = PartitionsT<InputArray, Element>;

    OutputPartitions output_partitions{shape_in_partitions(array)};
    AllPartitionAction<InputPartition, Element> action;

    for(std::size_t p = 0; p < nr_partitions(array); ++p) {

        output_partitions[p] = hpx::dataflow(
            hpx::launch::async,

            [action](
                InputPartition const& input_partition)
            {
                return action(
                    hpx::get_colocation_id(
                        hpx::launch::sync, input_partition.get_id()),
                    input_partition);
            },

            array.partitions()[p]);

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

                    // If one of the elements evaluates to false, then
                    // the result is false
                    if(!result) {
                        // Short-circuit
                        break;
                    }

                }

                return result;
            }

        ));
}

}  // namespace lue
