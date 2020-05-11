#pragma once
#include "lue/framework/core/component/partitioned_array.hpp"
#include <hpx/lcos/when_all.hpp>
#include <algorithm>
#include <random>


namespace lue {
namespace detail {
namespace uniform {

template<
    typename T,
    typename Enable=void>
class OverloadPicker
{
};


template<
    typename InputPartition,
    typename OutputElement>
PartitionT<InputPartition, OutputElement> uniform_partition(
    InputPartition const& input_partition,
    hpx::shared_future<OutputElement> const min_value,
    hpx::shared_future<OutputElement> const max_value)
{
    assert(
        hpx::get_colocation_id(input_partition.get_id()).get() ==
        hpx::find_here());

    using OutputPartition = PartitionT<InputPartition, OutputElement>;
    using OutputData = DataT<OutputPartition>;

    return hpx::dataflow(
        hpx::launch::async,

        [](
            InputPartition const& input_partition,
            hpx::shared_future<OutputElement> const& min_value,
            hpx::shared_future<OutputElement> const& max_value)
        {
            auto const input_partition_server_ptr{
                hpx::get_ptr(input_partition).get()};
            auto const& input_partition_server{
                *input_partition_server_ptr};

            auto const partition_offset = input_partition_server.offset();
            auto const partition_shape = input_partition_server.shape();

            // Will be used to obtain a seed for the random number engine
            std::random_device random_device;

            // Standard mersenne_twister_engine seeded with the random_device
            std::mt19937 random_number_engine(random_device());

            auto distribution =
                [min_value=min_value.get(), max_value=max_value.get()]()
                {
                    if constexpr(std::is_floating_point_v<OutputElement>) {
                        return std::uniform_real_distribution<OutputElement>{
                            min_value, max_value};
                    }
                    else if constexpr(std::is_integral_v<OutputElement>) {
                        return std::uniform_int_distribution<OutputElement>{
                            min_value, max_value};
                    }
                }();

            OutputData output_partition_data{partition_shape};

            std::generate(
                    output_partition_data.begin(),
                    output_partition_data.end(),

                    [&]()
                    {
                        return distribution(random_number_engine);
                    }

                );

            return OutputPartition{
                hpx::find_here(), partition_offset,
                std::move(output_partition_data)};

        },

        input_partition,
        min_value,
        max_value);
}


template<
    typename InputPartition,
    typename OutputElement>
struct UniformPartitionAction:
    hpx::actions::make_action<
        decltype(&uniform_partition<InputPartition, OutputElement>),
        &uniform_partition<InputPartition, OutputElement>,
        UniformPartitionAction<InputPartition, OutputElement>>
{};

}  // namespace uniform
}  // namespace detail


template<
    typename T>
using UniformAction =
    typename detail::uniform::OverloadPicker<T>::Action;


/*!
    @overload
*/
template<
    typename InputElement,
    typename OutputElement,
    Rank rank>
PartitionedArray<OutputElement, rank> uniform(
    PartitionedArray<InputElement, rank> const& input_array,
    hpx::shared_future<OutputElement> const& min_value,
    hpx::shared_future<OutputElement> const& max_value)
{
    // Reimplement as ternary local operation?

    // The result array will have the same shape, partitioning and
    // location as the input array, but the elements might be of a different
    // type.

    using InputArray = PartitionedArray<InputElement, rank>;
    using InputPartition = PartitionT<InputArray>;

    using OutputArray = PartitionedArray<OutputElement, rank>;
    using OutputPartitions = PartitionsT<OutputArray>;

    detail::uniform::UniformPartitionAction<
        InputPartition, OutputElement> action;
    OutputPartitions output_partitions{shape_in_partitions(input_array)};

    for(Index p = 0; p < nr_partitions(input_array); ++p) {

        InputPartition const& input_partition{
            input_array.partitions()[p]};

        output_partitions[p] = hpx::dataflow(
            hpx::launch::async,
            hpx::util::unwrapping(

                    [action, input_partition, min_value, max_value](
                        hpx::id_type const locality_id)
                    {
                        return action(
                            locality_id,
                            input_partition, min_value, max_value);
                    }

                ),
            hpx::get_colocation_id(input_partition.get_id()));
    }

    return OutputArray{shape(input_array), std::move(output_partitions)};
}


/*!
    @overload
*/
template<
    typename InputElement,
    typename OutputElement,
    Rank rank>
PartitionedArray<OutputElement, rank> uniform(
    PartitionedArray<InputElement, rank> const& input_array,
    OutputElement const min_value,
    OutputElement const max_value)
{
    return uniform(
        input_array,
        hpx::make_ready_future<OutputElement>(min_value).share(),
        hpx::make_ready_future<OutputElement>(max_value).share());
}

}  // namespace lue
