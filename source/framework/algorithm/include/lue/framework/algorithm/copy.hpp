#pragma once
#include "lue/framework/core/component/partitioned_array.hpp"
#include "lue/framework/core/type_traits.hpp"
#include <hpx/include/lcos.hpp>


namespace lue {
namespace detail {

/*!
    @brief      Return a new copy the array partition passed in
    @tparam     Partition Client class of partition component
    @param      input_partition Client of array partition component
    @return     A copy of the array partition passed in
*/
template<
    typename Partition>
Partition copy_partition(
    Partition const& input_partition)
{
    using Offset = OffsetT<Partition>;
    using InputData = DataT<Partition>;

    lue_assert(input_partition.is_ready());

    return hpx::dataflow(
        hpx::launch::async,
        hpx::util::unwrapping(

                [input_partition](
                    Offset const& offset,
                    InputData&& input_partition_data)
                {
                    HPX_UNUSED(input_partition);

                    // Copy the data and move it into a new partition
                    return Partition{hpx::find_here(), offset, std::move(input_partition_data)};
                }

            ),
        input_partition.offset(),
        input_partition.data());
}

}  // namespace detail


template<
    typename Partition>
struct CopyPartitionAction:
    hpx::actions::make_action<
        decltype(&detail::copy_partition<Partition>),
        &detail::copy_partition<Partition>,
        CopyPartitionAction<Partition>>
{};


// template<
//     typename Element,
//     Rank rank>
// ArrayPartition<Element, rank> copy(
//     ArrayPartition<Element, rank> const& input_partition)
// {
//     using Partition = ArrayPartition<Element, rank>;
// 
//     CopyPartitionAction<Partition> action;
// 
//     return hpx::dataflow(
//         hpx::launch::async,
//         hpx::util::unwrapping(
// 
//                 [action, input_partition](
//                     hpx::id_type const locality_id)
//                 {
//                     return action(locality_id, input_partition);
//                 }
// 
//             ),
//         input_partition.locality_id());
// }


/*!
    @brief      Return the result of copying a partitioned array
    @tparam     Element Type of elements in the arrays
    @tparam     rank Rank of the input array
    @param      array Partitioned array to copy
    @return     New partitioned array
*/
template<
    typename Element,
    Rank rank>
PartitionedArray<Element, rank> copy(
    PartitionedArray<Element, rank> const& input_array)
{
    using InputArray = PartitionedArray<Element, rank>;
    using InputPartitions = PartitionsT<InputArray>;
    using InputPartition = PartitionT<InputArray>;

    using OutputArray = PartitionedArray<Element, rank>;
    using OutputPartitions = PartitionsT<OutputArray>;

    CopyPartitionAction<InputPartition> action;
    Localities<rank> const& localities{input_array.localities()};
    InputPartitions const& input_partitions{input_array.partitions()};
    OutputPartitions output_partitions{shape_in_partitions(input_array)};

    for(Index p = 0; p < nr_partitions(input_array); ++p)
    {
        output_partitions[p] = hpx::dataflow(
            hpx::launch::async,

            [locality_id=localities[p], action](
                InputPartition const& input_partition)
            {
                return action(locality_id, input_partition);
            },

            input_partitions[p]);
    }

    return OutputArray{shape(input_array), localities, std::move(output_partitions)};
}

}  // namespace lue
