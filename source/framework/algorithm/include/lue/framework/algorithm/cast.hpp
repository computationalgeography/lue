#pragma once
#include "lue/framework/core/component/array_partition.hpp"
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

    hpx::future<OutputData> cast = hpx::dataflow(
        hpx::launch::async,
        hpx::util::unwrapping(
            [](
                InputData const& input_partition_data)
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

                return output_partition_data;
            }
        ),
        input_partition.data(CopyMode::share)
    );

    hpx::id_type partition_id = input_partition.get_id();

    return cast.then(
        hpx::util::unwrapping(
            [partition_id](
                OutputData&& output_partition_data)
            {
                return OutputPartition{partition_id, output_partition_data};
            }
        )
    );
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

        InputPartition const& input_partition = array.partitions()[p];

        output_partitions[p] =
            hpx::get_colocation_id(input_partition.get_id()).then(
                hpx::util::unwrapping(
                    [=](
                        hpx::naming::id_type const locality_id)
                    {
                        return hpx::dataflow(
                            hpx::launch::async,
                            action,
                            locality_id,
                            input_partition);
                    }
                )
            );
    }

    return OutputArray{shape(array), std::move(output_partitions)};
}

}  // namespace lue
