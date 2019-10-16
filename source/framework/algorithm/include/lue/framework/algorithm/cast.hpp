#pragma once
#include "lue/framework/core/type_traits.hpp"
#include <hpx/include/lcos.hpp>


namespace lue {
namespace detail {

template<
    typename InputPartition,
    typename OutputElement>
PartitionT<InputPartition, OutputElement> cast_partition(
    InputPartition const& input_partition)
{
    assert(
        hpx::get_colocation_id(input_partition.get_id()).get() ==
        hpx::find_here());

    using InputData = DataT<InputPartition>;
    using InputElement = ElementT<InputData>;

    using OutputPartition = PartitionT<InputPartition, OutputElement>;
    using OutputData = DataT<OutputPartition>;

    return hpx::dataflow(
        hpx::launch::async,
        hpx::util::unwrapping(

            [](
                hpx::id_type const locality_id,
                InputData&& input_partition_data)
            {
                OutputData output_partition_data{input_partition_data.shape()};

                std::transform(
                    input_partition_data.begin(),
                    input_partition_data.end(),
                    output_partition_data.begin(),
                    [](
                        InputElement const element)
                    {
                        return static_cast<OutputElement>(element);
                    });

                return OutputPartition{
                    locality_id, std::move(output_partition_data)};
            }

        ),
        hpx::get_colocation_id(input_partition.get_id()),
        input_partition.data(CopyMode::share));
}

}  // namespace detail


template<
    typename InputPartition,
    typename OutputElement>
struct CastPartitionAction:
    hpx::actions::make_action<
        decltype(&detail::cast_partition<InputPartition, OutputElement>),
        &detail::cast_partition<InputPartition, OutputElement>,
        CastPartitionAction<InputPartition, OutputElement>>
{};


/*!
    @brief      Return the result of casting a partitioned array to
                another type
    @tparam     OutputElement Type of elements in the output array
    @tparam     InputElement Type of elements in the input array
    @tparam     rank Rank of the input array
    @tparam     Array Class template of the type of the arrays
    @param      array Partitioned array
    @return     New partitioned array
*/
template<
    typename OutputElement,
    typename InputElement,
    std::size_t rank,
    template<typename, std::size_t> typename Array>
Array<OutputElement, rank> cast(
    Array<InputElement, rank> const& array)
{
    using InputArray = Array<InputElement, rank>;
    using InputPartition = PartitionT<InputArray>;

    using OutputArray = Array<OutputElement, rank>;
    using OutputPartitions = PartitionsT<OutputArray>;

    CastPartitionAction<InputPartition, OutputElement> action;
    OutputPartitions output_partitions{shape_in_partitions(array)};

    for(std::size_t p = 0; p < nr_partitions(array); ++p) {

        output_partitions[p] = hpx::dataflow(
            hpx::launch::async,
            hpx::util::unwrapping(

                [action](
                    hpx::id_type const component_id)
                {
                    return action(
                        hpx::get_colocation_id(
                            hpx::launch::sync, component_id),
                        InputPartition{component_id});
                }

            ),
            array.partitions()[p]);

    }

    return OutputArray{shape(array), std::move(output_partitions)};
}

}  // namespace lue
