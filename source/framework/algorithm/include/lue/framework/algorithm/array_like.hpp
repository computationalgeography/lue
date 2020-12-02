#pragma once
#include "lue/framework/core/component/partitioned_array.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"


namespace lue {
namespace detail {

template<
    typename Policies,
    typename InputElement,
    typename OutputElement,
    Rank rank>
ArrayPartition<OutputElement, rank> array_like_partition(
    Policies const& policies,
    ArrayPartition<InputElement, rank> const& input_partition,
    OutputElement const fill_value)
{
    using InputPartition = ArrayPartition<InputElement, rank>;
    using Offset = OffsetT<InputPartition>;
    using Shape = ShapeT<InputPartition>;

    using OutputPartition = ArrayPartition<OutputElement, rank>;
    using OutputData = DataT<OutputPartition>;

    lue_assert(input_partition.is_ready());

    return hpx::dataflow(
        hpx::launch::async,
        hpx::util::unwrapping(

                [policies, input_partition, fill_value](
                    Offset const& offset,
                    Shape const& shape)
                {
                    AnnotateFunction annotation{"array_like_partition"};

                    HPX_UNUSED(input_partition);

                    OutputData output_partition_data{shape};

                    auto const& [indp] = policies.input_no_data_policies();
                    auto const& [ondp] = policies.output_no_data_policies();

                    Count const nr_elements{lue::nr_elements(output_partition_data)};

                    if(indp.is_no_data(fill_value))
                    {
                        for(Index i = 0; i < nr_elements; ++i)
                        {
                            ondp.mark_no_data(output_partition_data, i);
                        }
                    }
                    else
                    {
                        for(Index i = 0; i < nr_elements; ++i)
                        {
                            output_partition_data[i] = fill_value;
                        }
                    }

                    return OutputPartition{hpx::find_here(), offset, std::move(output_partition_data)};
                }

            ),
        input_partition.offset(),
        input_partition.shape());
}

}  // namespace detail


namespace policy {
namespace array_like {

using DefaultPolicies = policy::DefaultPolicies<1, 1>;

}  // namespace array_like
}  // namespace policy


template<
    typename Policies,
    typename InputElement,
    typename OutputElement,
    Rank rank>
struct ArrayLikePartitionAction:
    hpx::actions::make_action<
        decltype(&detail::array_like_partition<Policies, InputElement, OutputElement, rank>),
        &detail::array_like_partition<Policies, InputElement, OutputElement, rank>,
        ArrayLikePartitionAction<Policies, InputElement, OutputElement, rank>>
{};


template<
    typename Policies,
    typename OutputElement,
    typename InputElement,
    Rank rank>
PartitionedArray<OutputElement, rank> array_like(
    Policies const& policies,
    PartitionedArray<InputElement, rank> const& input_array,
    hpx::shared_future<OutputElement> const& fill_value)
{
    using InputArray = PartitionedArray<InputElement, rank>;
    using InputPartitions = PartitionsT<InputArray>;
    using InputPartition = PartitionT<InputArray>;

    using OutputArray = PartitionedArray<OutputElement, rank>;
    using OutputPartitions = PartitionsT<OutputArray>;

    ArrayLikePartitionAction<Policies, InputElement, OutputElement, rank> action;

    Localities<rank> const& localities{input_array.localities()};
    InputPartitions const& input_partitions{input_array.partitions()};
    OutputPartitions output_partitions{shape_in_partitions(input_array)};

    for(Index p = 0; p < nr_partitions(input_array); ++p)
    {
        output_partitions[p] = hpx::dataflow(
            hpx::launch::async,

            [locality_id=localities[p], action, policies](
                InputPartition const& input_partition,
                hpx::shared_future<OutputElement> const& fill_value)
            {
                AnnotateFunction annotation{"array_like"};

                return action(locality_id, policies, input_partition, fill_value.get());
            },

            input_partitions[p],
            fill_value);
    }

    return OutputArray{shape(input_array), localities, std::move(output_partitions)};
}


template<
    typename OutputElement,
    typename InputElement,
    Rank rank>
PartitionedArray<OutputElement, rank> array_like(
    PartitionedArray<InputElement, rank> const& input_array,
    OutputElement const fill_value)
{
    return array_like(
        policy::array_like::DefaultPolicies{},
        input_array, hpx::make_ready_future<OutputElement>(fill_value).share());
}

}  // namespace lue
