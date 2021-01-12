#pragma once
#include "lue/framework/algorithm/policy/default_policies.hpp"
#include "lue/framework/algorithm/detail/halo_partition.hpp"
#include "lue/framework/algorithm/flow_direction.hpp"
#include "lue/framework/core/component.hpp"


namespace lue {
namespace detail {
namespace inflow_count {

template<
    typename Element>
ArrayPartition<Element, 2> corner_halo_partition(
    Element const fill_value)
{
    // Asynchronously create a new partition:
    // - With a shape based on the partition passsed in
    // - Filled with the fill_value passed in

    using Partition = ArrayPartition<Element, 2>;
    using Shape = ShapeT<Partition>;
    using Offset = OffsetT<Partition>;

    return Partition{hpx::find_here(), Offset{}, Shape{{1, 1}}, fill_value};
}


template<
    typename Element>
ArrayPartition<Element, 2> longitudinal_side_halo_partition(
    ArrayPartition<Element, 2> const& partition,
    Element const fill_value)
{
    // Asynchronously create a new partition:
    // - With a shape based on the partition passsed in
    // - Filled with the fill_value passed in

    using Partition = ArrayPartition<Element, 2>;
    using Shape = ShapeT<Partition>;
    using Offset = OffsetT<Partition>;

    return hpx::dataflow(
        hpx::launch::async,
        hpx::util::unwrapping(

                [fill_value](
                    Shape const& shape)
                {
                    return Partition{hpx::find_here(), Offset{}, Shape{{1, shape[1]}}, fill_value};
                }

            ),
        partition.shape());
}


template<
    typename Element>
ArrayPartition<Element, 2> latitudinal_side_halo_partition(
    ArrayPartition<Element, 2> const& partition,
    Element const fill_value)
{
    // Asynchronously create a new partition:
    // - With a shape based on the partition passsed in
    // - Filled with the fill_value passed in

    using Partition = ArrayPartition<Element, 2>;
    using Shape = ShapeT<Partition>;
    using Offset = OffsetT<Partition>;

    return hpx::dataflow(
        hpx::launch::async,
        hpx::util::unwrapping(

                [fill_value](
                    Shape const& shape)
                {
                    return Partition{hpx::find_here(), Offset{}, Shape{{shape[0], 1}}, fill_value};
                }

            ),
        partition.shape());
}


template<
    typename Element,
    Rank rank>
hpx::future<Array<Element, rank>> when_all_get(
    Array<hpx::shared_future<Element>, rank>&& array_of_futures)
{
    // Given an array of futures, wait for all futures to become ready
    // and return an array with the values pointed to by the futures.

    return hpx::when_all(array_of_futures.begin(), array_of_futures.end()).then(
        [shape=array_of_futures.shape()](
            hpx::future<std::vector<hpx::shared_future<Element>>>&& f)
        {
            std::vector<hpx::shared_future<Element>> vector_of_ready_futures{f.get()};
            Array<Element, rank> array_of_elements{{shape}};

            std::transform(
                    vector_of_ready_futures.begin(), vector_of_ready_futures.end(),
                    array_of_elements.begin(),

                    [](
                        hpx::shared_future<Element> const& future)
                    {
                        return future.get();
                    }
                );

            return array_of_elements;
        });
}


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
    typename FlowDirectionData>
std::tuple<Index, Index> downstream_cell(
    FlowDirectionData const& flow_direction,
    Index const idx0,
    Index const idx1)
{
    using FlowDirectionElement = ElementT<FlowDirectionData>;

    FlowDirectionElement const flow_direction_element{flow_direction(idx0, idx1)};

    Index offset0{0};
    Index offset1{0};

    if(flow_direction_element == north_west<FlowDirectionElement>)
    {
        offset0 = -1;
        offset1 = -1;
    }
    else if(flow_direction_element == north<FlowDirectionElement>)
    {
        offset0 = -1;
    }
    else if(flow_direction_element == north_east<FlowDirectionElement>)
    {
        offset0 = -1;
        offset1 = 1;
    }
    else if(flow_direction_element == west<FlowDirectionElement>)
    {
        offset1 = -1;
    }
    else if(flow_direction_element == east<FlowDirectionElement>)
    {
        offset1 = 1;
    }
    else if(flow_direction_element == south_west<FlowDirectionElement>)
    {
        offset0 = 1;
        offset1 = -1;
    }
    else if(flow_direction_element == south<FlowDirectionElement>)
    {
        offset0 = 1;
    }
    else if(flow_direction_element == south_east<FlowDirectionElement>)
    {
        offset0 = 1;
        offset1 = 1;
    }

    return {idx0 + offset0, idx1 + offset1};
}


template<
    typename Policies,
    typename InflowCountPartition,
    typename FlowDirectionPartitions>
InflowCountPartition inflow_count_partition(
    Policies const& policies,
    FlowDirectionPartitions const& flow_direction_partitions)
{
    using FlowDirectionPartition = PartitionT<FlowDirectionPartitions>;
    Rank const rank{lue::rank<FlowDirectionPartition>};
    using FlowDirectionData = DataT<FlowDirectionPartition>;
    using FlowDirectionPartitionsData = Array<hpx::shared_future<FlowDirectionData>, rank>;

    using Shape = ShapeT<FlowDirectionPartition>;

    Shape const& shape_in_partitions{flow_direction_partitions.shape()};
    auto const [nr_partitions0, nr_partitions1] = shape_in_partitions;

    using Offset = OffsetT<FlowDirectionPartition>;
    hpx::future<Offset> offset{flow_direction_partitions(1, 1).offset()};

    using PartitionShapes = Array<hpx::shared_future<Shape>, rank>;

    lue_assert(all_are_ready(flow_direction_partitions));
    // hpx::wait_all(flow_direction_partitions.begin(), flow_direction_partitions.end());

    // Determine the shape of all partitions -----------------------------------
    PartitionShapes partition_shapes{shape_in_partitions};

    {
        for(Index idx0 = 0; idx0 < nr_partitions0; ++idx0) {
            for(Index idx1 = 0; idx1 < nr_partitions1; ++idx1)
            {
                partition_shapes(idx0, idx1) = flow_direction_partitions(idx0, idx1).shape();
            }
        }
    }

    // Request the minimum amount of required data from all partitions ---------
    FlowDirectionPartitionsData flow_direction_data{shape_in_partitions};

    {
        using Slice = SliceT<FlowDirectionPartition>;
        using Slices = SlicesT<FlowDirectionPartition>;

        // North-west corner partition: get south-east corner element
        flow_direction_data(0, 0) = partition_shapes(0, 0).then(hpx::util::unwrapping(

                [flow_direction_partition=flow_direction_partitions(0, 0)](
                    Shape const& partition_shape)
                {
                    auto const [nr_elements0, nr_elements1] = partition_shape;

                    return flow_direction_partition.slice(
                        Slices{{
                            Slice{nr_elements0 - 1, nr_elements0},
                            Slice{nr_elements1 - 1, nr_elements1}
                        }});
                }

            ));

        // North partition: get south side elements
        flow_direction_data(0, 1) = partition_shapes(0, 1).then(hpx::util::unwrapping(

                [input_partition=flow_direction_partitions(0, 1)](
                    Shape const& partition_shape)
                {
                    auto const [nr_elements0, nr_elements1] = partition_shape;

                    return input_partition.slice(
                        Slices{{
                            Slice{nr_elements0 - 1, nr_elements0},
                            Slice{0, nr_elements1}
                        }});
                }

            ));

        // North-east partition: get south-west corner elements
        flow_direction_data(0, 2) = partition_shapes(0, 2).then(hpx::util::unwrapping(

                [input_partition=flow_direction_partitions(0, 2)](
                    Shape const& partition_shape)
                {
                    auto const [nr_elements0, nr_elements1] = partition_shape;

                    return input_partition.slice(
                        Slices{{
                            Slice{nr_elements0 - 1, nr_elements0},
                            Slice{0, 1}
                        }});
                }

            ));

        // West partition: get east side elements
        flow_direction_data(1, 0) = partition_shapes(1, 0).then(hpx::util::unwrapping(

                [input_partition=flow_direction_partitions(1, 0)](
                    Shape const& partition_shape)
                {
                    auto const [nr_elements0, nr_elements1] = partition_shape;

                    return input_partition.slice(
                        Slices{{
                            Slice{0, nr_elements0},
                            Slice{nr_elements1 - 1, nr_elements1}
                        }});
                }

            ));

        // Center partition: get all elements
        flow_direction_data(1, 1) = flow_direction_partitions(1, 1).data();

        // East partition: get west side elements
        flow_direction_data(1, 2) = partition_shapes(1, 2).then(hpx::util::unwrapping(

                [input_partition=flow_direction_partitions(1, 2)](
                    Shape const& partition_shape)
                {
                    auto const [nr_elements0, nr_elements1] = partition_shape;

                    return input_partition.slice(
                        Slices{{
                            Slice{0, nr_elements0},
                            Slice{0, 1}
                        }});
                }

            ));

        // South-west partition: get north-east corner elements
        flow_direction_data(2, 0) = partition_shapes(2, 0).then(hpx::util::unwrapping(

                [input_partition=flow_direction_partitions(2, 0)](
                    Shape const& partition_shape)
                {
                    auto const [nr_elements0, nr_elements1] = partition_shape;

                    return input_partition.slice(
                        Slices{{
                            Slice{0, 1},
                            Slice{nr_elements1 - 1, nr_elements1}
                        }});
                }

            ));

        // South partition: get north side elements
        flow_direction_data(2, 1) = partition_shapes(2, 1).then(hpx::util::unwrapping(

                [input_partition=flow_direction_partitions(2, 1)](
                    Shape const& partition_shape)
                {
                    auto const [nr_elements0, nr_elements1] = partition_shape;

                    return input_partition.slice(
                        Slices{{
                            Slice{0, 1},
                            Slice{0, nr_elements1}
                        }});
                }

            ));

        // South-east partition: get north-west corner element
        flow_direction_data(2, 2) = flow_direction_partitions(2, 2).slice(
            Slices{{
                Slice{0, 1},
                Slice{0, 1}
            }});

        lue_assert(all_are_valid(flow_direction_data));
    }

    using InflowCountData = DataT<InflowCountPartition>;
    using FlowDirectionElement = ElementT<FlowDirectionData>;

    // Once the elements from the center partition have arrived,
    // handle all cells whose downstream cells are contained within
    // this partition
    hpx::future<InflowCountData> inflow_count_data = hpx::dataflow(
        hpx::launch::async,
        hpx::util::unwrapping(

                [policies](
                    FlowDirectionData const& flow_direction_data)
                {
                    auto const& indp{std::get<0>(policies.inputs_policies()).input_no_data_policy()};
                    auto const& ondp{std::get<0>(policies.outputs_policies()).output_no_data_policy()};

                    Shape const& partition_shape{flow_direction_data.shape()};
                    auto const [nr_elements0, nr_elements1] = partition_shape;
                    InflowCountData output_data{partition_shape, 0};

                    for(Index idx0 = 1; idx0 < nr_elements0 - 1; ++idx0) {
                        for(Index idx1 = 1; idx1 < nr_elements1 - 1; ++idx1)
                        {
                            if(indp.is_no_data(flow_direction_data, idx0, idx1))
                            {
                                ondp.mark_no_data(output_data, idx0, idx1);
                            }
                            else
                            {
                                auto const [rd, cd] = downstream_cell(flow_direction_data, idx0, idx1);

                                if(rd != idx0 || cd != idx1)
                                {
                                    // Current cell is no a sink. In
                                    // a valid flow direction network,
                                    // the destination cell cannot contain
                                    // no-data.
                                    lue_assert(!indp.is_no_data(flow_direction_data(rd, cd)));
                                    output_data(rd, cd) += 1;
                                }
                            }
                        }
                    }

                    // North-west cell
                    {
                        Index const idx0{0};
                        Index const idx1{0};

                        if(indp.is_no_data(flow_direction_data, idx0, idx1))
                        {
                            ondp.mark_no_data(output_data, idx0, idx1);
                        }
                        else
                        {
                            auto const flow_direction{flow_direction_data(idx0, idx1)};

                            if(flow_direction == east<FlowDirectionElement>)
                            {
                                lue_assert(!indp.is_no_data(flow_direction_data(idx0, idx1 + 1)));
                                output_data(idx0, idx1 + 1) += 1;
                            }
                            else if(flow_direction == south_east<FlowDirectionElement>)
                            {
                                lue_assert(!indp.is_no_data(flow_direction_data(idx0 + 1, idx1 + 1)));
                                output_data(idx0 + 1, idx1 + 1) += 1;
                            }
                            else if(flow_direction == south<FlowDirectionElement>)
                            {
                                lue_assert(!indp.is_no_data(flow_direction_data(idx0 + 1, idx1)));
                                output_data(idx0 + 1, idx1) += 1;
                            }
                        }
                    }

                    // North-east cell
                    {
                        Index const idx0{0};
                        Index const idx1{nr_elements1 - 1};

                        if(indp.is_no_data(flow_direction_data, idx0, idx1))
                        {
                            ondp.mark_no_data(output_data, idx0, idx1);
                        }
                        else
                        {
                            auto const flow_direction{flow_direction_data(idx0, idx1)};

                            if(flow_direction == west<FlowDirectionElement>)
                            {
                                lue_assert(!indp.is_no_data(flow_direction_data(idx0, idx1 - 1)));
                                output_data(idx0, idx1 - 1) += 1;
                            }
                            else if(flow_direction == south_west<FlowDirectionElement>)
                            {
                                lue_assert(!indp.is_no_data(flow_direction_data(idx0 + 1, idx1 - 1)));
                                output_data(idx0 + 1, idx1 - 1) += 1;
                            }
                            else if(flow_direction == south<FlowDirectionElement>)
                            {
                                lue_assert(!indp.is_no_data(flow_direction_data(idx0 + 1, idx1)));
                                output_data(idx0 + 1, idx1) += 1;
                            }
                        }
                    }

                    // South-west cell
                    {
                        Index const idx0{nr_elements0 - 1};
                        Index const idx1{0};

                        if(indp.is_no_data(flow_direction_data, idx0, idx1))
                        {
                            ondp.mark_no_data(output_data, idx0, idx1);
                        }
                        else
                        {
                            auto const flow_direction{flow_direction_data(idx0, idx1)};

                            if(flow_direction == north<FlowDirectionElement>)
                            {
                                lue_assert(!indp.is_no_data(flow_direction_data(idx0 - 1, idx1)));
                                output_data(idx0 - 1, idx1) += 1;
                            }
                            else if(flow_direction == north_east<FlowDirectionElement>)
                            {
                                lue_assert(!indp.is_no_data(flow_direction_data(idx0 - 1, idx1 + 1)));
                                output_data(idx0 - 1, idx1 + 1) += 1;
                            }
                            else if(flow_direction == east<FlowDirectionElement>)
                            {
                                lue_assert(!indp.is_no_data(flow_direction_data(idx0, idx1 + 1)));
                                output_data(idx0, idx1 + 1) += 1;
                            }
                        }
                    }

                    // South-east cell
                    {
                        Index const idx0{nr_elements0 - 1};
                        Index const idx1{nr_elements1 - 1};

                        if(indp.is_no_data(flow_direction_data, idx0, idx1))
                        {
                            ondp.mark_no_data(output_data, idx0, idx1);
                        }
                        else
                        {
                            auto const flow_direction{flow_direction_data(idx0, idx1)};

                            if(flow_direction == west<FlowDirectionElement>)
                            {
                                lue_assert(!indp.is_no_data(flow_direction_data(idx0, idx1 - 1)));
                                output_data(idx0, idx1 - 1) += 1;
                            }
                            else if(flow_direction == north_west<FlowDirectionElement>)
                            {
                                lue_assert(!indp.is_no_data(flow_direction_data(idx0 - 1, idx1 - 1)));
                                output_data(idx0 - 1, idx1 - 1) += 1;
                            }
                            else if(flow_direction == north<FlowDirectionElement>)
                            {
                                lue_assert(!indp.is_no_data(flow_direction_data(idx0 - 1, idx1)));
                                output_data(idx0 - 1, idx1) += 1;
                            }
                        }
                    }

                    if(nr_elements1 > 2)
                    {
                        // North cells
                        {
                            Index const idx0 = 0;

                            for(Index idx1 = 1; idx1 < nr_elements1 - 1; ++idx1)
                            {
                                if(indp.is_no_data(flow_direction_data, idx0, idx1))
                                {
                                    ondp.mark_no_data(output_data, idx0, idx1);
                                }
                                else
                                {
                                    auto const flow_direction{flow_direction_data(idx0, idx1)};

                                    if(flow_direction == west<FlowDirectionElement>)
                                    {
                                        lue_assert(!indp.is_no_data(flow_direction_data(idx0, idx1 - 1)));
                                        output_data(idx0, idx1 - 1) += 1;
                                    }
                                    else if(flow_direction == south_west<FlowDirectionElement>)
                                    {
                                        lue_assert(!indp.is_no_data(flow_direction_data(idx0 + 1, idx1 - 1)));
                                        output_data(idx0 + 1, idx1 - 1) += 1;
                                    }
                                    else if(flow_direction == south<FlowDirectionElement>)
                                    {
                                        lue_assert(!indp.is_no_data(flow_direction_data(idx0 + 1, idx1)));
                                        output_data(idx0 + 1, idx1) += 1;
                                    }
                                    else if(flow_direction == south_east<FlowDirectionElement>)
                                    {
                                        lue_assert(!indp.is_no_data(flow_direction_data(idx0 + 1, idx1 + 1)));
                                        output_data(idx0 + 1, idx1 + 1) += 1;
                                    }
                                    else if(flow_direction == east<FlowDirectionElement>)
                                    {
                                        lue_assert(!indp.is_no_data(flow_direction_data(idx0, idx1 + 1)));
                                        output_data(idx0, idx1 + 1) += 1;
                                    }
                                }
                            }
                        }

                        // South cells
                        {
                            Index const idx0 = nr_elements0 - 1;

                            for(Index idx1 = 1; idx1 < nr_elements1 - 1; ++idx1)
                            {
                                if(indp.is_no_data(flow_direction_data, idx0, idx1))
                                {
                                    ondp.mark_no_data(output_data, idx0, idx1);
                                }
                                else
                                {
                                    auto const flow_direction{flow_direction_data(idx0, idx1)};

                                    if(flow_direction == west<FlowDirectionElement>)
                                    {
                                        lue_assert(!indp.is_no_data(flow_direction_data(idx0, idx1 - 1)));
                                        output_data(idx0, idx1 - 1) += 1;
                                    }
                                    else if(flow_direction == north_west<FlowDirectionElement>)
                                    {
                                        lue_assert(!indp.is_no_data(flow_direction_data(idx0 - 1, idx1 - 1)));
                                        output_data(idx0 - 1, idx1 - 1) += 1;
                                    }
                                    else if(flow_direction == north<FlowDirectionElement>)
                                    {
                                        lue_assert(!indp.is_no_data(flow_direction_data(idx0 - 1, idx1)));
                                        output_data(idx0 - 1, idx1) += 1;
                                    }
                                    else if(flow_direction == north_east<FlowDirectionElement>)
                                    {
                                        lue_assert(!indp.is_no_data(flow_direction_data(idx0 - 1, idx1 + 1)));
                                        output_data(idx0 - 1, idx1 + 1) += 1;
                                    }
                                    else if(flow_direction == east<FlowDirectionElement>)
                                    {
                                        lue_assert(!indp.is_no_data(flow_direction_data(idx0, idx1 + 1)));
                                        output_data(idx0, idx1 + 1) += 1;
                                    }
                                }
                            }
                        }
                    }

                    if(nr_elements0 > 2)
                    {
                        // West cells
                        {
                            Index const idx1 = 0;

                            for(Index idx0 = 1; idx0 < nr_elements0 - 1; ++idx0)
                            {
                                if(indp.is_no_data(flow_direction_data, idx0, idx1))
                                {
                                    ondp.mark_no_data(output_data, idx0, idx1);
                                }
                                else
                                {
                                    auto const flow_direction{flow_direction_data(idx0, idx1)};

                                    if(flow_direction == north<FlowDirectionElement>)
                                    {
                                        lue_assert(!indp.is_no_data(flow_direction_data(idx0 - 1, idx1)));
                                        output_data(idx0 - 1, idx1) += 1;
                                    }
                                    else if(flow_direction == north_east<FlowDirectionElement>)
                                    {
                                        lue_assert(!indp.is_no_data(flow_direction_data(idx0 - 1, idx1 + 1)));
                                        output_data(idx0 - 1, idx1 + 1) += 1;
                                    }
                                    else if(flow_direction == east<FlowDirectionElement>)
                                    {
                                        lue_assert(!indp.is_no_data(flow_direction_data(idx0, idx1 + 1)));
                                        output_data(idx0, idx1 + 1) += 1;
                                    }
                                    else if(flow_direction == south_east<FlowDirectionElement>)
                                    {
                                        lue_assert(!indp.is_no_data(flow_direction_data(idx0 + 1, idx1 + 1)));
                                        output_data(idx0 + 1, idx1 + 1) += 1;
                                    }
                                    else if(flow_direction == south<FlowDirectionElement>)
                                    {
                                        lue_assert(!indp.is_no_data(flow_direction_data(idx0 + 1, idx1)));
                                        output_data(idx0 + 1, idx1) += 1;
                                    }
                                }
                            }
                        }

                        // East cells
                        {
                            Index const idx1 = nr_elements1 - 1;

                            for(Index idx0 = 1; idx0 < nr_elements0 - 1; ++idx0)
                            {
                                if(indp.is_no_data(flow_direction_data, idx0, idx1))
                                {
                                    ondp.mark_no_data(output_data, idx0, idx1);
                                }
                                else
                                {
                                    auto const flow_direction{flow_direction_data(idx0, idx1)};

                                    if(flow_direction == north<FlowDirectionElement>)
                                    {
                                        lue_assert(!indp.is_no_data(flow_direction_data(idx0 - 1, idx1)));
                                        output_data(idx0 - 1, idx1) += 1;
                                    }
                                    else if(flow_direction == north_west<FlowDirectionElement>)
                                    {
                                        lue_assert(!indp.is_no_data(flow_direction_data(idx0 - 1, idx1 - 1)));
                                        output_data(idx0 - 1, idx1 - 1) += 1;
                                    }
                                    else if(flow_direction == west<FlowDirectionElement>)
                                    {
                                        lue_assert(!indp.is_no_data(flow_direction_data(idx0, idx1 - 1)));
                                        output_data(idx0, idx1 - 1) += 1;
                                    }
                                    else if(flow_direction == south_west<FlowDirectionElement>)
                                    {
                                        lue_assert(!indp.is_no_data(flow_direction_data(idx0 + 1, idx1 - 1)));
                                        output_data(idx0 + 1, idx1 - 1) += 1;
                                    }
                                    else if(flow_direction == south<FlowDirectionElement>)
                                    {
                                        lue_assert(!indp.is_no_data(flow_direction_data(idx0 + 1, idx1)));
                                        output_data(idx0 + 1, idx1) += 1;
                                    }
                                }
                            }
                        }
                    }

                    return output_data;
                }

            ),
        flow_direction_data(1, 1));

    // Once the elements from all neighbouring partitions have arrived,
    // handle all cells whose downstream cells are contained within
    // the center partition
    return hpx::dataflow(
        hpx::launch::async,
        hpx::util::unwrapping(

                [policies](
                    Offset offset,
                    lue::Array<FlowDirectionData, rank> const& flow_direction_data,
                    InflowCountData inflow_count_data)
                {
                    auto const& indp{std::get<0>(policies.inputs_policies()).input_no_data_policy()};
                    auto const& ondp{std::get<0>(policies.outputs_policies()).output_no_data_policy()};

                    // Note: all output no-data cells have already been marked

                    Array<Shape, 2> partition_shapes{Shape{{3, 3}}};

                    {
                        std::transform(
                                flow_direction_data.begin(), flow_direction_data.end(),
                                partition_shapes.begin(),

                                [](
                                    FlowDirectionData const& data)
                                {
                                    return data.shape();
                                }

                            );
                    }

                    // Extents of the inner partition
                    auto const [nr_elements0, nr_elements1] = partition_shapes(1, 1);

                    // North-west corner
                    {
                        FlowDirectionData const& data{flow_direction_data(0, 0)};

                        lue_assert(data.shape()[0] == 1);
                        lue_assert(data.shape()[1] == 1);

                        if(!indp.is_no_data(data, 0, 0))
                        {
                            auto const flow_direction{data(0, 0)};

                            if(flow_direction == south_east<FlowDirectionElement>)
                            {
                                lue_assert(!ondp.is_no_data(inflow_count_data, 0, 0));
                                inflow_count_data(0, 0) += 1;
                            }
                        }
                    }

                    // North-east corner
                    {
                        FlowDirectionData const& data{flow_direction_data(0, 2)};

                        lue_assert(data.shape()[0] == 1);
                        lue_assert(data.shape()[1] == 1);

                        if(!indp.is_no_data(data, 0, 0))
                        {
                            auto const flow_direction{data(0, 0)};

                            if(flow_direction == south_west<FlowDirectionElement>)
                            {
                                lue_assert(!ondp.is_no_data(inflow_count_data, 0, nr_elements1 - 1));
                                inflow_count_data(0, nr_elements1 - 1) += 1;
                            }
                        }
                    }

                    // South-west corner
                    {
                        FlowDirectionData const& data{flow_direction_data(2, 0)};

                        lue_assert(data.shape()[0] == 1);
                        lue_assert(data.shape()[1] == 1);

                        if(!indp.is_no_data(data, 0, 0))
                        {
                            auto const flow_direction{data(0, 0)};

                            if(flow_direction == north_east<FlowDirectionElement>)
                            {
                                lue_assert(!ondp.is_no_data(inflow_count_data, nr_elements0 - 1, 0));
                                inflow_count_data(nr_elements0 - 1, 0) += 1;
                            }
                        }
                    }

                    // South-east corner
                    {
                        FlowDirectionData const& data{flow_direction_data(2, 2)};

                        lue_assert(data.shape()[0] == 1);
                        lue_assert(data.shape()[1] == 1);

                        if(!indp.is_no_data(data, 0, 0))
                        {
                            auto const flow_direction{data(0, 0)};

                            if(flow_direction == north_west<FlowDirectionElement>)
                            {
                                lue_assert(!ondp.is_no_data(
                                    inflow_count_data, nr_elements0 - 1, nr_elements1 - 1));
                                inflow_count_data(nr_elements0 - 1, nr_elements1 - 1) += 1;
                            }
                        }
                    }

                    // North side
                    {
                        FlowDirectionData const& data{flow_direction_data(0, 1)};

                        lue_assert(data.shape()[0] == 1);
                        lue_assert(data.shape()[1] == nr_elements1);

                        Index const idx0 = 0;

                        {
                            Index const idx1 = 0;

                            if(!indp.is_no_data(data, 0, idx1))
                            {
                                auto const flow_direction{data(0, idx1)};

                                if(flow_direction == south<FlowDirectionElement>)
                                {
                                    lue_assert(!ondp.is_no_data(inflow_count_data, idx0, idx1));
                                    inflow_count_data(idx0, idx1) += 1;
                                }
                                else if(flow_direction == south_east<FlowDirectionElement>)
                                {
                                    lue_assert(!ondp.is_no_data(inflow_count_data, idx0, idx1 + 1));
                                    inflow_count_data(idx0, idx1 + 1) += 1;
                                }
                            }
                        }

                        if(nr_elements1 > 1)
                        {
                            Index const idx1 = nr_elements1 - 1;

                            if(!indp.is_no_data(data, 0, idx1))
                            {
                                auto const flow_direction{data(0, idx1)};

                                if(flow_direction == south_west<FlowDirectionElement>)
                                {
                                    lue_assert(!ondp.is_no_data(inflow_count_data, idx0, idx1 - 1));
                                    inflow_count_data(idx0, idx1 - 1) += 1;
                                }
                                else if(flow_direction == south<FlowDirectionElement>)
                                {
                                    lue_assert(!ondp.is_no_data(inflow_count_data, idx0, idx1));
                                    inflow_count_data(idx0, idx1) += 1;
                                }
                            }
                        }

                        if(nr_elements1 > 2)
                        {
                            for(Index idx1 = 1; idx1 < nr_elements1 - 1; ++idx1)
                            {
                                if(!indp.is_no_data(data, 0, idx1))
                                {
                                    auto const flow_direction{data(0, idx1)};

                                    if(flow_direction == south_west<FlowDirectionElement>)
                                    {
                                        lue_assert(!ondp.is_no_data(inflow_count_data, idx0, idx1 - 1));
                                        inflow_count_data(idx0, idx1 - 1) += 1;
                                    }
                                    else if(flow_direction == south<FlowDirectionElement>)
                                    {
                                        lue_assert(!ondp.is_no_data(inflow_count_data, idx0, idx1));
                                        inflow_count_data(idx0, idx1) += 1;
                                    }
                                    else if(flow_direction == south_east<FlowDirectionElement>)
                                    {
                                        lue_assert(!ondp.is_no_data(inflow_count_data, idx0, idx1 + 1));
                                        inflow_count_data(idx0, idx1 + 1) += 1;
                                    }
                                }
                            }
                        }
                    }

                    // South side
                    {
                        FlowDirectionData const& data{flow_direction_data(2, 1)};

                        lue_assert(data.shape()[0] == 1);
                        lue_assert(data.shape()[1] == nr_elements1);

                        Index const idx0 = nr_elements0 - 1;

                        {
                            Index const idx1 = 0;

                            if(!indp.is_no_data(data, 0, idx1))
                            {
                                auto const flow_direction{data(0, idx1)};

                                if(flow_direction == north<FlowDirectionElement>)
                                {
                                    lue_assert(!ondp.is_no_data(inflow_count_data, idx0, idx1));
                                    inflow_count_data(idx0, idx1) += 1;
                                }
                                else if(flow_direction == north_east<FlowDirectionElement>)
                                {
                                    lue_assert(!ondp.is_no_data(inflow_count_data, idx0, idx1 + 1));
                                    inflow_count_data(idx0, idx1 + 1) += 1;
                                }
                            }
                        }

                        if(nr_elements1 > 1)
                        {
                            Index const idx1 = nr_elements1 - 1;

                            if(!indp.is_no_data(data, 0, idx1))
                            {
                                auto const flow_direction{data(0, idx1)};

                                if(flow_direction == north_west<FlowDirectionElement>)
                                {
                                    lue_assert(!ondp.is_no_data(inflow_count_data, idx0, idx1 - 1));
                                    inflow_count_data(idx0, idx1 - 1) += 1;
                                }
                                else if(flow_direction == north<FlowDirectionElement>)
                                {
                                    lue_assert(!ondp.is_no_data(inflow_count_data, idx0, idx1));
                                    inflow_count_data(idx0, idx1) += 1;
                                }
                            }
                        }

                        if(nr_elements1 > 2)
                        {
                            for(Index idx1 = 1; idx1 < nr_elements1 - 1; ++idx1)
                            {
                                if(!indp.is_no_data(data, 0, idx1))
                                {
                                    auto const flow_direction{data(0, idx1)};

                                    if(flow_direction == north_west<FlowDirectionElement>)
                                    {
                                        lue_assert(!ondp.is_no_data(inflow_count_data, idx0, idx1 - 1));
                                        inflow_count_data(idx0, idx1 - 1) += 1;
                                    }
                                    else if(flow_direction == north<FlowDirectionElement>)
                                    {
                                        lue_assert(!ondp.is_no_data(inflow_count_data, idx0, idx1));
                                        inflow_count_data(idx0, idx1) += 1;
                                    }
                                    else if(flow_direction == north_east<FlowDirectionElement>)
                                    {
                                        lue_assert(!ondp.is_no_data(inflow_count_data, idx0, idx1 + 1));
                                        inflow_count_data(idx0, idx1 + 1) += 1;
                                    }
                                }
                            }
                        }
                    }

                    // West side
                    {
                        FlowDirectionData const& data{flow_direction_data(1, 0)};

                        lue_assert(data.shape()[0] == nr_elements0);
                        lue_assert(data.shape()[1] == 1);

                        Index const idx1 = 0;

                        {
                            Index const idx0 = 0;

                            if(!indp.is_no_data(data, idx0, 0))
                            {
                                auto const flow_direction{data(idx0, 0)};

                                if(flow_direction == east<FlowDirectionElement>)
                                {
                                    lue_assert(!ondp.is_no_data(inflow_count_data, idx0, idx1));
                                    inflow_count_data(idx0, idx1) += 1;
                                }
                                else if(flow_direction == south_east<FlowDirectionElement>)
                                {
                                    lue_assert(!ondp.is_no_data(inflow_count_data, idx0 + 1, idx1));
                                    inflow_count_data(idx0 + 1, idx1) += 1;
                                }
                            }
                        }

                        if(nr_elements0 > 1)
                        {
                            Index const idx0 = nr_elements0 - 1;

                            if(!indp.is_no_data(data, idx0, 0))
                            {
                                auto const flow_direction{data(idx0, 0)};

                                if(flow_direction == north_east<FlowDirectionElement>)
                                {
                                    lue_assert(!ondp.is_no_data(inflow_count_data, idx0 - 1, idx1));
                                    inflow_count_data(idx0 - 1, idx1) += 1;
                                }
                                else if(flow_direction == east<FlowDirectionElement>)
                                {
                                    lue_assert(!ondp.is_no_data(inflow_count_data, idx0, idx1));
                                    inflow_count_data(idx0, idx1) += 1;
                                }
                            }
                        }

                        if(nr_elements0 > 2)
                        {
                            for(Index idx0 = 1; idx0 < nr_elements0 - 1; ++idx0)
                            {
                                if(!indp.is_no_data(data, idx0, 0))
                                {
                                    auto const flow_direction{data(idx0, 0)};

                                    if(flow_direction == north_east<FlowDirectionElement>)
                                    {
                                        lue_assert(!ondp.is_no_data(inflow_count_data, idx0 - 1, idx1));
                                        inflow_count_data(idx0 - 1, idx1) += 1;
                                    }
                                    else if(flow_direction == east<FlowDirectionElement>)
                                    {
                                        lue_assert(!ondp.is_no_data(inflow_count_data, idx0, idx1));
                                        inflow_count_data(idx0, idx1) += 1;
                                    }
                                    else if(flow_direction == south_east<FlowDirectionElement>)
                                    {
                                        lue_assert(!ondp.is_no_data(inflow_count_data, idx0 + 1, idx1));
                                        inflow_count_data(idx0 + 1, idx1) += 1;
                                    }
                                }
                            }
                        }
                    }

                    // East side
                    {
                        FlowDirectionData const& data{flow_direction_data(1, 2)};

                        lue_assert(data.shape()[0] == nr_elements0);
                        lue_assert(data.shape()[1] == 1);

                        Index const idx1 = nr_elements1 - 1;

                        {
                            Index const idx0 = 0;

                            if(!indp.is_no_data(data, idx0, 0))
                            {
                                auto const flow_direction{data(idx0, 0)};

                                if(flow_direction == west<FlowDirectionElement>)
                                {
                                    lue_assert(!ondp.is_no_data(inflow_count_data, idx0, idx1));
                                    inflow_count_data(idx0, idx1) += 1;
                                }
                                else if(flow_direction == south_west<FlowDirectionElement>)
                                {
                                    lue_assert(!ondp.is_no_data(inflow_count_data, idx0 + 1, idx1));
                                    inflow_count_data(idx0 + 1, idx1) += 1;
                                }
                            }
                        }

                        if(nr_elements0 > 1)
                        {
                            Index const idx0 = nr_elements0 - 1;

                            if(!indp.is_no_data(data, idx0, 0))
                            {
                                auto const flow_direction{data(idx0, 0)};

                                if(flow_direction == north_west<FlowDirectionElement>)
                                {
                                    lue_assert(!ondp.is_no_data(inflow_count_data, idx0 - 1, idx1));
                                    inflow_count_data(idx0 - 1, idx1) += 1;
                                }
                                else if(flow_direction == west<FlowDirectionElement>)
                                {
                                    lue_assert(!ondp.is_no_data(inflow_count_data, idx0, idx1));
                                    inflow_count_data(idx0, idx1) += 1;
                                }
                            }
                        }

                        if(nr_elements0 > 2)
                        {
                            for(Index idx0 = 1; idx0 < nr_elements0 - 1; ++idx0)
                            {
                                if(!indp.is_no_data(data, idx0, 0))
                                {
                                    auto const flow_direction{data(idx0, 0)};

                                    if(flow_direction == north_west<FlowDirectionElement>)
                                    {
                                        lue_assert(!ondp.is_no_data(inflow_count_data, idx0 - 1, idx1));
                                        inflow_count_data(idx0 - 1, idx1) += 1;
                                    }
                                    else if(flow_direction == west<FlowDirectionElement>)
                                    {
                                        lue_assert(!ondp.is_no_data(inflow_count_data, idx0, idx1));
                                        inflow_count_data(idx0, idx1) += 1;
                                    }
                                    else if(flow_direction == south_west<FlowDirectionElement>)
                                    {
                                        lue_assert(!ondp.is_no_data(inflow_count_data, idx0 + 1, idx1));
                                        inflow_count_data(idx0 + 1, idx1) += 1;
                                    }
                                }
                            }
                        }
                    }

                    return InflowCountPartition{hpx::find_here(), offset, std::move(inflow_count_data)};
                }

            ),
        std::move(offset),
        when_all_get(std::move(flow_direction_data)),
        std::move(inflow_count_data));
}


template<
    typename Element>
struct CornerHaloPartitionAction:
    hpx::actions::make_action<
        decltype(&corner_halo_partition<Element>),
        &corner_halo_partition<Element>,
        CornerHaloPartitionAction<Element>>
{};


template<
    typename Element>
struct LongitudinalSideHaloPartitionAction:
    hpx::actions::make_action<
        decltype(&longitudinal_side_halo_partition<Element>),
        &longitudinal_side_halo_partition<Element>,
        LongitudinalSideHaloPartitionAction<Element>>
{};


template<
    typename Element>
struct LatitudinalSideHaloPartitionAction:
    hpx::actions::make_action<
        decltype(&latitudinal_side_halo_partition<Element>),
        &latitudinal_side_halo_partition<Element>,
        LatitudinalSideHaloPartitionAction<Element>>
{};


template<
    typename Policies,
    typename InflowCountPartition,
    typename FlowDirectionPartitions>
struct InflowCountPartitionAction:
    hpx::actions::make_action<
        decltype(&inflow_count_partition<Policies, InflowCountPartition, FlowDirectionPartitions>),
        &inflow_count_partition<Policies, InflowCountPartition, FlowDirectionPartitions>,
        InflowCountPartitionAction<Policies, InflowCountPartition, FlowDirectionPartitions>>
{};

}  // namespace inflow_count


template<
    typename InflowCountPartition,
    typename FlowDirectionPartitions,
    typename Policies,
    typename Action>
InflowCountPartition spawn_inflow_count_partition(
    hpx::id_type const locality_id,
    Action const& action,
    Policies const& policies,
    FlowDirectionPartitions&& flow_direction_partitions)
{
    // Attach a continuation to the future which becomes ready once all
    // input partitions have become ready.
    return hpx::dataflow(
        hpx::launch::async,
        hpx::util::unwrapping(

                [locality_id, action, policies](
                    FlowDirectionPartitions const& flow_direction_partitions)
                {
                    return action(locality_id, policies, flow_direction_partitions);
                }

            ),
        inflow_count::when_all_get(std::move(flow_direction_partitions)));
}

}  // namespace detail


namespace policy {
namespace inflow_count {

template<
    typename CountElement,
    typename FlowDirectionElement>
using DefaultPolicies = policy::DefaultPolicies<
    OutputElements<CountElement>,
    InputElements<FlowDirectionElement>>;

}  // namespace inflow_count
}  // namespace policy


template<
    typename Policies,
    typename CountElement,
    typename FlowDirectionElement,
    Rank rank>
PartitionedArray<CountElement, rank> inflow_count(
    Policies const& policies,
    PartitionedArray<FlowDirectionElement, rank> const& flow_direction)
{
    // The result of this function must be equal to
    // upstream(flow_direction, material=1), but is should be faster
    // (less memory accesses)

    using FlowDirectionArray = PartitionedArray<FlowDirectionElement, rank>;
    using FlowDirectionPartitions = PartitionsT<FlowDirectionArray>;

    using InflowCountArray = PartitionedArray<CountElement, rank>;
    using InflowCountPartition = PartitionT<InflowCountArray>;
    using InflowCountPartitions = PartitionsT<InflowCountArray>;

    FlowDirectionPartitions const& flow_direction_partitions{flow_direction.partitions()};
    Localities<rank> const& localities{flow_direction.localities()};

    lue_assert(all_are_valid(flow_direction_partitions));

    auto const [nr_partitions0, nr_partitions1] = lue::shape_in_partitions(flow_direction);

    // -------------------------------------------------------------------------
    // Create halo partitions with flow directions away from the array

    FlowDirectionPartitions halo_corner_partitions{{2, 2}};
    {
        // Corner halo partitions
        //     +----+----+
        //     | NW | NE |
        //     +----+----+
        //     | SW | SE |
        //     +----+----+

        using Action = detail::inflow_count::CornerHaloPartitionAction<FlowDirectionElement>;
        Action action{};

        // North-west corner halo partition
        halo_corner_partitions(0, 0) =
            hpx::async(action, localities(0, 0), north_west<FlowDirectionElement>);

        // North-east corner halo partition
        halo_corner_partitions(0, 1) =
            hpx::async(action, localities(0, nr_partitions1 - 1), north_east<FlowDirectionElement>);

        // South-west corner halo partition
        halo_corner_partitions(1, 0) =
            hpx::async(action, localities(nr_partitions0 - 1, 0), south_west<FlowDirectionElement>);

        // South-east corner halo partition
        halo_corner_partitions(1, 1) =
            hpx::async(action, localities(nr_partitions0 - 1, nr_partitions1 - 1),
                south_east<FlowDirectionElement>);
    }

    FlowDirectionPartitions halo_longitudinal_side_partitions{{2, nr_partitions1}};
    {
        // Longitudinal side halo partitions
        //     +---+---+---+
        //     | N | N | N |
        //     +---+---+---+
        //     | S | S | S |
        //     +---+---+---+

        using Action = detail::inflow_count::LongitudinalSideHaloPartitionAction<FlowDirectionElement>;
        Action action{};

        for(auto const [rh, rp, halo_flow_direction]: {
                std::tuple<Index, Index, FlowDirectionElement>{
                    0, 0, north<FlowDirectionElement>},
                std::tuple<Index, Index, FlowDirectionElement>{
                    1, nr_partitions0 - 1, south<FlowDirectionElement>}
            })
        {
            for(Index cp = 0; cp < nr_partitions1; ++cp)
            {
                halo_longitudinal_side_partitions(rh, cp) =
                    hpx::async(action, localities(nr_partitions0 - 1, nr_partitions1 - 1),
                        flow_direction_partitions(rp, cp), halo_flow_direction);
            }
        }
    }

    FlowDirectionPartitions halo_latitudinal_side_partitions{{nr_partitions0, 2}};
    {
        // Latitudinal sides halo partitions
        //     +---+---+
        //     | W | E |
        //     +---+---+
        //     | W | E |
        //     +---+---+
        //     | W | E |
        //     +---+---+

        using Action = detail::inflow_count::LatitudinalSideHaloPartitionAction<FlowDirectionElement>;
        Action action{};

        for(Index rp = 0; rp < nr_partitions0; ++rp) {

            for(auto const [ch, cp, halo_flow_direction]: {
                    std::tuple<Index, Index, FlowDirectionElement>{
                        0, 0, west<FlowDirectionElement>},
                    std::tuple<Index, Index, FlowDirectionElement>{
                        1, nr_partitions1 - 1, east<FlowDirectionElement>}
                })
            {
                halo_latitudinal_side_partitions(rp, ch) =
                    hpx::async(action, localities(rp, cp),
                        flow_direction_partitions(rp, cp), halo_flow_direction);
            }
        }
    }

    lue_assert(all_are_valid(halo_corner_partitions));
    lue_assert(all_are_valid(halo_longitudinal_side_partitions));
    lue_assert(all_are_valid(halo_latitudinal_side_partitions));

    // -------------------------------------------------------------------------
    // Attach continuations to groups of nine partitions

    // When handling the partitions at the side of the array, halo
    // partitions must be used

    InflowCountPartitions inflow_count_partitions{shape_in_partitions(flow_direction)};

    {
        auto const [nr_partitions0, nr_partitions1] = lue::shape_in_partitions(flow_direction);
        lue_assert(nr_partitions0 > 0);
        lue_assert(nr_partitions1 > 0);

        using Action =
            detail::inflow_count::InflowCountPartitionAction<
                Policies, InflowCountPartition, FlowDirectionPartitions>;
        Action action{};

        // North-west partition
        inflow_count_partitions(0, 0) = detail::spawn_inflow_count_partition<InflowCountPartition>(
            localities(0, 0), action, policies,
            detail::north_west_corner_input_partitions(
                flow_direction_partitions, halo_corner_partitions,
                halo_longitudinal_side_partitions, halo_latitudinal_side_partitions));

        if(nr_partitions1 > 1)
        {
            // North-east partition
            inflow_count_partitions(0, nr_partitions1 - 1) =
                detail::spawn_inflow_count_partition<InflowCountPartition>(
                    localities(0, nr_partitions1 - 1), action, policies,
                    detail::north_east_corner_input_partitions(
                        flow_direction_partitions, halo_corner_partitions,
                        halo_longitudinal_side_partitions, halo_latitudinal_side_partitions));
        }

        if(nr_partitions0 > 1)
        {
            // South-west partition
            inflow_count_partitions(nr_partitions0 - 1, 0) =
                detail::spawn_inflow_count_partition<InflowCountPartition>(
                    localities(nr_partitions0 - 1, 0), action, policies,
                    detail::south_west_corner_input_partitions(
                        flow_direction_partitions, halo_corner_partitions,
                        halo_longitudinal_side_partitions, halo_latitudinal_side_partitions));
        }

        if(nr_partitions0 > 1 && nr_partitions1 > 1)
        {
            // South-east partition
            inflow_count_partitions(nr_partitions0 - 1, nr_partitions1 - 1) =
                detail::spawn_inflow_count_partition<InflowCountPartition>(
                    localities(nr_partitions0 - 1, nr_partitions1 - 1), action, policies,
                    detail::south_east_corner_input_partitions(
                        flow_direction_partitions, halo_corner_partitions,
                        halo_longitudinal_side_partitions, halo_latitudinal_side_partitions));
        }

        for(Index c = 1; c < nr_partitions1 - 1; ++c)
        {
            // North-side partitions
            inflow_count_partitions(0, c) =
                detail::spawn_inflow_count_partition<InflowCountPartition>(
                    localities(0, c), action, policies,
                    detail::north_side_input_partitions(
                        c, flow_direction_partitions, halo_longitudinal_side_partitions));
        }

        if(nr_partitions0 > 1)
        {
            // South-side partitions
            for(Index c = 1; c < nr_partitions1 - 1; ++c)
            {
                inflow_count_partitions(nr_partitions0 - 1, c) =
                    detail::spawn_inflow_count_partition<InflowCountPartition>(
                        localities(nr_partitions0 - 1, c), action, policies,
                        detail::south_side_input_partitions(
                            c, flow_direction_partitions, halo_longitudinal_side_partitions));
            }
        }

        // West-side partitions
        for(Index r = 1; r < nr_partitions0 - 1; ++r)
        {
            inflow_count_partitions(r, 0) =
                detail::spawn_inflow_count_partition<InflowCountPartition>(
                    localities(r, 0), action, policies,
                    detail::west_side_input_partitions(
                        r, flow_direction_partitions, halo_latitudinal_side_partitions));
        }

        if(nr_partitions1 > 1)
        {
            // East-side partitions
            for(Index r = 1; r < nr_partitions0 - 1; ++r)
            {
                inflow_count_partitions(r, nr_partitions1 - 1) =
                    detail::spawn_inflow_count_partition<InflowCountPartition>(
                        localities(r, nr_partitions1 - 1), action, policies,
                        detail::east_side_input_partitions(
                            r, flow_direction_partitions, halo_latitudinal_side_partitions));
            }
        }

        // r, c is the center partition
        for(Index r = 1; r < nr_partitions0 - 1; ++r) {
            for(Index c = 1; c < nr_partitions1 - 1; ++c)
            {
                // Center partitions
                inflow_count_partitions(r, c) =
                    detail::spawn_inflow_count_partition<InflowCountPartition>(
                        localities(r, c), action, policies,
                        detail::inner_input_partitions(r, c, flow_direction_partitions));
            }
        }
    }

    lue_assert(all_are_valid(inflow_count_partitions));

    return InflowCountArray{flow_direction.shape(), localities, std::move(inflow_count_partitions)};
}


template<
    typename CountElement,
    typename FlowDirectionElement,
    Rank rank>
PartitionedArray<CountElement, rank> inflow_count(
    PartitionedArray<FlowDirectionElement, rank> const& flow_direction)
{
    using Policies = policy::inflow_count::DefaultPolicies<CountElement, FlowDirectionElement>;

    return inflow_count<Policies, CountElement, FlowDirectionElement>(Policies{}, flow_direction);
}

}  // namespace lue
