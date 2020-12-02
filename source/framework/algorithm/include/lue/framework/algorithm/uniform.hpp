#pragma once
#include "lue/framework/algorithm/policy/default_policies.hpp"
#include "lue/framework/core/component/partitioned_array.hpp"
#include "lue/framework/core/component.hpp"
#include <algorithm>
#include <random>


namespace lue {
namespace detail {
namespace uniform {

template<
    typename Policies,
    typename InputPartition,
    typename OutputElement>
PartitionT<InputPartition, OutputElement> uniform_partition(
    Policies const& policies,
    InputPartition const& input_partition,
    OutputElement const min_value,
    OutputElement const max_value)
{
    using Offset = OffsetT<InputPartition>;
    using Shape = ShapeT<InputPartition>;
    using OutputPartition = PartitionT<InputPartition, OutputElement>;
    using OutputData = DataT<OutputPartition>;

    static_assert(std::is_floating_point_v<OutputElement> || std::is_integral_v<OutputElement>);

    lue_assert(input_partition.is_ready());

    return hpx::dataflow(
        hpx::launch::async,
        hpx::util::unwrapping(

                [policies, input_partition, min_value, max_value](
                    Offset const& offset,
                    Shape const& shape)
                {
                    AnnotateFunction annotation{"uniform_partition"};

                    HPX_UNUSED(input_partition);

                    OutputData output_partition_data{shape};

                    auto const& [indp1, indp2] = policies.input_no_data_policies();
                    auto const& [ondp] = policies.output_no_data_policies();

                    if(indp1.is_no_data(min_value) || indp2.is_no_data(max_value))
                    {
                        Count const nr_elements{lue::nr_elements(output_partition_data)};

                        for(Index i = 0; i < nr_elements; ++i)
                        {
                            ondp.mark_no_data(output_partition_data, i);
                        }
                    }
                    else
                    {
                        // Will be used to obtain a seed for the random number engine
                        std::random_device random_device;

                        // Standard mersenne_twister_engine seeded with the random_device
                        std::mt19937 random_number_engine(random_device());

                        auto distribution =
                            [min_value, max_value]()
                            {
                                if constexpr(std::is_floating_point_v<OutputElement>) {
                                    return std::uniform_real_distribution<OutputElement>{min_value, max_value};
                                }
                                else if constexpr(std::is_integral_v<OutputElement>) {
                                    return std::uniform_int_distribution<OutputElement>{min_value, max_value};
                                }
                            }();

                        std::generate(output_partition_data.begin(), output_partition_data.end(),

                                [&distribution, &random_number_engine]()
                                {
                                    return distribution(random_number_engine);
                                }

                            );
                    }

                    return OutputPartition{hpx::find_here(), offset, std::move(output_partition_data)};
                }

            ),
        input_partition.offset(),
        input_partition.shape());
}


template<
    typename Policies,
    typename InputPartition,
    typename OutputElement>
struct UniformPartitionAction:
    hpx::actions::make_action<
        decltype(&uniform_partition<Policies, InputPartition, OutputElement>),
        &uniform_partition<Policies, InputPartition, OutputElement>,
        UniformPartitionAction<Policies, InputPartition, OutputElement>>
{};

}  // namespace uniform
}  // namespace detail


/// template<
///     typename Policies,
///     typename T>
/// using UniformAction = typename detail::uniform::OverloadPicker<Policies, T>::Action;


namespace policy {
namespace uniform {

// The min/max values are the arguments whose values are relevant
using DefaultPolicies = policy::DefaultPolicies<2, 1>;

}  // namespace uniform
}  // namespace policy


template<
    typename Policies,
    typename InputElement,
    typename OutputElement,
    Rank rank>
PartitionedArray<OutputElement, rank> uniform(
    Policies const& policies,
    PartitionedArray<InputElement, rank> const& input_array,
    hpx::shared_future<OutputElement> const& min_value,
    hpx::shared_future<OutputElement> const& max_value)
{
    // Reimplement as ternary local operation?

    // The result array will have the same shape, partitioning and
    // location as the input array, but the elements might be of a different
    // type.

    using InputArray = PartitionedArray<InputElement, rank>;
    using InputPartitions = PartitionsT<InputArray>;
    using InputPartition = PartitionT<InputArray>;

    using OutputArray = PartitionedArray<OutputElement, rank>;
    using OutputPartitions = PartitionsT<OutputArray>;

    static_assert(std::is_floating_point_v<OutputElement> || std::is_integral_v<OutputElement>);

    lue_assert(all_are_valid(input_array.partitions()));
    lue_assert(min_value.valid());
    lue_assert(max_value.valid());

    detail::uniform::UniformPartitionAction<Policies, InputPartition, OutputElement> action;

    Localities<rank> const& localities{input_array.localities()};
    InputPartitions const& input_partitions{input_array.partitions()};
    OutputPartitions output_partitions{shape_in_partitions(input_array)};

    for(Index p = 0; p < nr_partitions(input_array); ++p)
    {
        output_partitions[p] = hpx::dataflow(
            hpx::launch::async,

            [locality_id=localities[p], action, policies](
                InputPartition const& input_partition,
                hpx::shared_future<OutputElement> const& min_value,
                hpx::shared_future<OutputElement> const& max_value)
            {
                AnnotateFunction annotation{"uniform"};
                return action(locality_id, policies, input_partition, min_value.get(), max_value.get());
            },

            input_partitions[p],
            min_value,
            max_value);
    }

    return OutputArray{shape(input_array), localities, std::move(output_partitions)};
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
    hpx::shared_future<OutputElement> const& min_value,
    hpx::shared_future<OutputElement> const& max_value)
{
    using Policies = policy::uniform::DefaultPolicies;

    return uniform(Policies{}, input_array, min_value, max_value);
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
