#pragma once
#include "lue/framework/algorithm/detail/halo_partition.hpp"
// #include "lue/framework/algorithm/policy/detail/type_list.hpp"
#include "lue/framework/core/array_partition_data.hpp"


namespace lue {
namespace detail {

template<
    typename Component,
    Rank rank>
hpx::future<ArrayPartitionData<Component, rank>> when_all_get(
    ArrayPartitionData<Component, rank>&& array_of_components)
{
    // Given an array of components, wait for all components to become ready
    // and return an array with the ready components.
    // hpx::when_all returns a linear vector. We want a nD array.

    return hpx::when_all(array_of_components.begin(), array_of_components.end()).then(
        [shape=array_of_components.shape()](
            hpx::future<std::vector<Component>>&& f)
        {
            std::vector<Component> vector_of_ready_components{f.get()};
            ArrayPartitionData<Component, rank> array_of_ready_components{{shape}};

            std::move(
                vector_of_ready_components.begin(), vector_of_ready_components.end(),
                array_of_ready_components.begin());

            return array_of_ready_components;
        });
}


template<
    typename OutputPartition,
    typename InputPartitions,
    typename Policies,
    typename Action>
OutputPartition spawn_partition(
    hpx::id_type const locality_id,
    Action const& action,
    Policies const& policies,
    OffsetT<OutputPartition> const& partition_offset,
    InputPartitions&& input_partitions)
{
    // Attach a continuation to the future which becomes ready once all
    // input partitions have become ready.
    return hpx::dataflow(
        hpx::launch::async,
        hpx::util::unwrapping(

                [locality_id, action, policies, partition_offset](
                    InputPartitions const& input_partitions)
                {
                    return action(locality_id, policies, partition_offset, input_partitions);
                }

            ),
        when_all_get(std::move(input_partitions)));
}


template<
    typename OutputPartition1,
    typename OutputPartition2,
    typename InputPartitions,
    typename Policies,
    typename Action>
hpx::tuple<OutputPartition1, OutputPartition2> spawn_partition(
    hpx::id_type const locality_id,
    Action const& action,
    Policies const& policies,
    OffsetT<OutputPartition1> const& partition_offset,
    InputPartitions&& input_partitions)
{
    // Attach a continuation to the future which becomes ready once all
    // input partitions have become ready.
    return hpx::split_future(hpx::dataflow(
        hpx::launch::async,
        hpx::util::unwrapping(

                [locality_id, action, policies, partition_offset](
                    InputPartitions const& input_partitions)
                {
                    return action(locality_id, policies, partition_offset, input_partitions);
                }

            ),
        when_all_get(std::move(input_partitions))));
}


template<
    typename Policies,
    typename OutputPartitions,
    typename InputPartitions,
    typename Action,
    typename Localities>
OutputPartitions spawn_partitions(
    Policies const& policies,
    Action const& action,
    Localities const& localities,
    InputPartitions const& input_partitions,
    InputPartitions const& halo_corner_partitions,
    InputPartitions const& halo_longitudinal_side_partitions,
    InputPartitions const& halo_latitudinal_side_partitions)
{
    using OutputPartition = PartitionT<OutputPartitions>;

    auto const& shape_in_partitions{input_partitions.shape()};
    OutputPartitions output_partitions{shape_in_partitions};

    auto const [nr_partitions0, nr_partitions1] = shape_in_partitions;
    lue_assert(nr_partitions0 > 0);
    lue_assert(nr_partitions1 > 0);

    using Offset = OffsetT<OutputPartition>;

    // Partition indices
    Index idx0, idx1;

    // North-west partition
    idx0 = 0;
    idx1 = 0;
    output_partitions(idx0, idx1) = spawn_partition<OutputPartition>(
        localities(idx0, idx1), action, policies, Offset{idx0, idx1},
        north_west_corner_input_partitions(
            input_partitions, halo_corner_partitions,
            halo_longitudinal_side_partitions, halo_latitudinal_side_partitions));

    if(nr_partitions1 > 1)
    {
        // North-east partition
        idx0 = 0;
        idx1 = nr_partitions1 - 1;
        output_partitions(idx0, idx1) = spawn_partition<OutputPartition>(
            localities(idx0, idx1), action, policies, Offset{idx0, idx1},
            north_east_corner_input_partitions(
                input_partitions, halo_corner_partitions,
                halo_longitudinal_side_partitions, halo_latitudinal_side_partitions));
    }

    if(nr_partitions0 > 1)
    {
        // South-west partition
        idx0 = nr_partitions0 - 1;
        idx1 = 0;
        output_partitions(idx0, idx1) = spawn_partition<OutputPartition>(
            localities(idx0, idx1), action, policies, Offset{idx0, idx1},
            south_west_corner_input_partitions(
                input_partitions, halo_corner_partitions,
                halo_longitudinal_side_partitions, halo_latitudinal_side_partitions));
    }

    if(nr_partitions0 > 1 && nr_partitions1 > 1)
    {
        // South-east partition
        idx0 = nr_partitions0 - 1;
        idx1 = nr_partitions1 - 1;
        output_partitions(idx0, idx1) = spawn_partition<OutputPartition>(
            localities(idx0, idx1), action, policies, Offset{idx0, idx1},
            south_east_corner_input_partitions(
                input_partitions, halo_corner_partitions,
                halo_longitudinal_side_partitions, halo_latitudinal_side_partitions));
    }

    for(idx0 = 0, idx1 = 1; idx1 < nr_partitions1 - 1; ++idx1)
    {
        // North-side partitions
        output_partitions(idx0, idx1) = spawn_partition<OutputPartition>(
            localities(idx0, idx1), action, policies, Offset{idx0, idx1},
            north_side_input_partitions(idx1, input_partitions, halo_longitudinal_side_partitions));
    }

    if(nr_partitions0 > 1)
    {
        // South-side partitions
        for(idx0 = nr_partitions0 - 1, idx1 = 1; idx1 < nr_partitions1 - 1; ++idx1)
        {
            output_partitions(idx0, idx1) = spawn_partition<OutputPartition>(
                localities(idx0, idx1), action, policies, Offset{idx0, idx1},
                south_side_input_partitions(idx1, input_partitions, halo_longitudinal_side_partitions));
        }
    }

    // West-side partitions
    for(idx0 = 1, idx1 = 0; idx0 < nr_partitions0 - 1; ++idx0)
    {
        output_partitions(idx0, idx1) = spawn_partition<OutputPartition>(
            localities(idx0, idx1), action, policies, Offset{idx0, idx1},
            west_side_input_partitions(idx0, input_partitions, halo_latitudinal_side_partitions));
    }

    if(nr_partitions1 > 1)
    {
        // East-side partitions
        for(idx0 = 1, idx1 = nr_partitions1 - 1; idx0 < nr_partitions0 - 1; ++idx0)
        {
            output_partitions(idx0, idx1) = spawn_partition<OutputPartition>(
                localities(idx0, idx1), action, policies, Offset{idx0, idx1},
                east_side_input_partitions(idx0, input_partitions, halo_latitudinal_side_partitions));
        }
    }

    // idx0, idx1 is the center partition
    for(idx0 = 1; idx0 < nr_partitions0 - 1; ++idx0) {
        for(idx1 = 1; idx1 < nr_partitions1 - 1; ++idx1)
        {
            // Center partitions
            output_partitions(idx0, idx1) = spawn_partition<OutputPartition>(
                localities(idx0, idx1), action, policies, Offset{idx0, idx1},
                inner_input_partitions(idx0, idx1, input_partitions));
        }
    }

    return output_partitions;
}


template<
    typename Policies,
    typename OutputPartitions1,
    typename OutputPartitions2,
    typename InputPartitions,
    typename Action,
    typename Localities>
std::tuple<OutputPartitions1, OutputPartitions2> spawn_partitions(
    Policies const& policies,
    Action const& action,
    Localities const& localities,
    InputPartitions const& input_partitions,
    InputPartitions const& halo_corner_partitions,
    InputPartitions const& halo_longitudinal_side_partitions,
    InputPartitions const& halo_latitudinal_side_partitions)
{
    using OutputPartition1 = PartitionT<OutputPartitions1>;
    using OutputPartition2 = PartitionT<OutputPartitions2>;

    auto const& shape_in_partitions{input_partitions.shape()};
    OutputPartitions1 output_partitions1{shape_in_partitions};
    OutputPartitions2 output_partitions2{shape_in_partitions};

    auto const [nr_partitions0, nr_partitions1] = shape_in_partitions;
    lue_assert(nr_partitions0 > 0);
    lue_assert(nr_partitions1 > 0);

    using Offset = OffsetT<OutputPartition1>;

    // Partition indices
    Index idx0, idx1;

    // North-west partition
    idx0 = 0;
    idx1 = 0;
    hpx::tie(output_partitions1(idx0, idx1), output_partitions2(idx0, idx1)) =
        spawn_partition<OutputPartition1, OutputPartition2>(localities(idx0, idx1),
            action, policies, Offset{idx0, idx1},
            north_west_corner_input_partitions(
                input_partitions, halo_corner_partitions,
                halo_longitudinal_side_partitions, halo_latitudinal_side_partitions));

    if(nr_partitions1 > 1)
    {
        // North-east partition
        idx0 = 0;
        idx1 = nr_partitions1 - 1;
        hpx::tie(output_partitions1(idx0, idx1), output_partitions2(idx0, idx1)) =
            spawn_partition<OutputPartition1, OutputPartition2>(localities(idx0, idx1),
                action, policies, Offset{idx0, idx1},
                north_east_corner_input_partitions(
                    input_partitions, halo_corner_partitions,
                    halo_longitudinal_side_partitions, halo_latitudinal_side_partitions));
    }

    if(nr_partitions0 > 1)
    {
        // South-west partition
        idx0 = nr_partitions0 - 1;
        idx1 = 0;
        hpx::tie(output_partitions1(idx0, idx1), output_partitions2(idx0, idx1)) =
            spawn_partition<OutputPartition1, OutputPartition2>(localities(idx0, idx1),
                action, policies, Offset{idx0, idx1},
                south_west_corner_input_partitions(
                    input_partitions, halo_corner_partitions,
                    halo_longitudinal_side_partitions, halo_latitudinal_side_partitions));
    }

    if(nr_partitions0 > 1 && nr_partitions1 > 1)
    {
        // South-east partition
        idx0 = nr_partitions0 - 1;
        idx1 = nr_partitions1 - 1;
        hpx::tie(output_partitions1(idx0, idx1), output_partitions2(idx0, idx1)) =
            spawn_partition<OutputPartition1, OutputPartition2>(localities(idx0, idx1),
                action, policies, Offset{idx0, idx1},
                south_east_corner_input_partitions(
                    input_partitions, halo_corner_partitions,
                    halo_longitudinal_side_partitions, halo_latitudinal_side_partitions));
    }

    for(idx0 = 0, idx1 = 1; idx1 < nr_partitions1 - 1; ++idx1)
    {
        // North-side partitions
        hpx::tie(output_partitions1(idx0, idx1), output_partitions2(idx0, idx1)) =
            spawn_partition<OutputPartition1, OutputPartition2>(localities(idx0, idx1),
                action, policies, Offset{idx0, idx1},
                north_side_input_partitions(idx1, input_partitions, halo_longitudinal_side_partitions));
    }

    if(nr_partitions0 > 1)
    {
        // South-side partitions
        for(idx0 = nr_partitions0 - 1, idx1 = 1; idx1 < nr_partitions1 - 1; ++idx1)
        {
            hpx::tie(output_partitions1(idx0, idx1), output_partitions2(idx0, idx1)) =
                spawn_partition<OutputPartition1, OutputPartition2>(localities(idx0, idx1),
                    action, policies, Offset{idx0, idx1},
                    south_side_input_partitions(idx1, input_partitions, halo_longitudinal_side_partitions));
        }
    }

    // West-side partitions
    for(idx0 = 1, idx1 = 0; idx0 < nr_partitions0 - 1; ++idx0)
    {
        hpx::tie(output_partitions1(idx0, idx1), output_partitions2(idx0, idx1)) =
            spawn_partition<OutputPartition1, OutputPartition2>(localities(idx0, idx1),
                action, policies, Offset{idx0, idx1},
                west_side_input_partitions(idx0, input_partitions, halo_latitudinal_side_partitions));
    }

    if(nr_partitions1 > 1)
    {
        // East-side partitions
        for(idx0 = 1, idx1 = nr_partitions1 - 1; idx0 < nr_partitions0 - 1; ++idx0)
        {
            hpx::tie(output_partitions1(idx0, idx1), output_partitions2(idx0, idx1)) =
                spawn_partition<OutputPartition1, OutputPartition2>(localities(idx0, idx1),
                    action, policies, Offset{idx0, idx1},
                    east_side_input_partitions(idx0, input_partitions, halo_latitudinal_side_partitions));
        }
    }

    // idx0, idx1 is the center partition
    for(idx0 = 1; idx0 < nr_partitions0 - 1; ++idx0) {
        for(idx1 = 1; idx1 < nr_partitions1 - 1; ++idx1)
        {
            // Center partitions
            hpx::tie(output_partitions1(idx0, idx1), output_partitions2(idx0, idx1)) =
                spawn_partition<OutputPartition1, OutputPartition2>(localities(idx0, idx1),
                    action, policies, Offset{idx0, idx1},
                    inner_input_partitions(idx0, idx1, input_partitions));
        }
    }

    return std::make_tuple(output_partitions1, output_partitions2);
}


/// template<
///     typename... OutputPartitions>
/// using OutputsPartitions = policy::detail::TypeList<OutputPartitions...>;
/// 
/// 
/// template<
///     typename Policies,
///     typename OutputPartitions,
///     typename InputPartitions,
///     typename Action,
///     typename Localities>
/// class ActionSpawner
/// {
/// };
/// 
/// 
/// template<
///     typename Policies,
///     typename... OutputPartitions,
///     typename InputPartitions,
///     typename Action,
///     typename Localities>
/// class ActionSpawner<
///     Policies,
///     policy::detail::TypeList<OutputPartitions...>,
///     InputPartitions,
///     Action,
///     Localities>
/// {
///     static std::tuple<OutputPartitions...> spawn_partitions(
///         Policies const& policies,
///         Action const& action,
///         Localities const& localities,
///         InputPartitions const& input_partitions,
///         InputPartitions const& halo_corner_partitions,
///         InputPartitions const& halo_longitudinal_side_partitions,
///         InputPartitions const& halo_latitudinal_side_partitions)
///     {
///         // TODO Make this all work in case of a single return and multiple returns from actions
///     }
/// };

}  // namespace detail
}  // namespace lue
