#pragma once
#include "lue/framework/algorithm/array_partition_io.hpp"
#include "lue/framework/algorithm/inflow_count.hpp"
#include "lue/framework/algorithm/detail/accumulate.hpp"


namespace lue {
namespace detail {
namespace inter_partition_stream {

template<
    typename InterPartitionStreamData,
    typename PartitionIOData>
class Accumulator
{

    public:

        using Index = typename InterPartitionStreamData::Shape::value_type;
        using CellsIdxs = Array<typename ElementT<PartitionIOData>::CellsIdxs, 2>;
        using StreamClass = ElementT<InterPartitionStreamData>;
        using Count = typename ElementT<PartitionIOData>::Value;
        using CountData = Array<Count, 2>;

        // _count_data contains for each cell the number of cells that
        // flow towards it + 1 for the cell itself (within the partition)
        // Afterwards, at each partition output cell it is known how
        // many cells contribute to its value / length in number of
        // cells of stream

        Accumulator(
            CellsIdxs& cells_idxs,
            InterPartitionStreamData& stream_data,
            PartitionIOData& partition_io_data):

            _cells_idxs{cells_idxs},
            _stream_data{stream_data},
            _stream_class{},
            _partition_io_data{partition_io_data},
            _count_data{stream_data.shape(), 1}

        {
            lue_assert(partition_io_data.shape()[0] == 1);
            lue_assert(partition_io_data.shape()[1] == 1);
        }

        void accumulate(
            Index const idx0_from,
            Index const idx1_from,
            Index const idx0_to,
            Index const idx1_to)
        {
            // Move all cell indices from the source collection to the
            // destination collection
            std::vector<std::array<Index, 2>>& from_collection{_cells_idxs(idx0_from, idx1_from)};
            std::vector<std::array<Index, 2>>& to_collection{_cells_idxs(idx0_to, idx1_to)};

            to_collection.insert(
                to_collection.end(),
                std::make_move_iterator(from_collection.begin()),
                std::make_move_iterator(from_collection.end()));
            from_collection.clear();

            _stream_data(idx0_to, idx1_to) = _stream_class;
            _count_data(idx0_to, idx1_to) += _count_data(idx0_from, idx1_from);
        }

        void at_partition_output_cell(
            Index const idx0,
            Index const idx1)
        {
            auto& cells_idxs{_cells_idxs(idx0, idx1)};
            std::sort(cells_idxs.begin(), cells_idxs.end());
            _partition_io_data(0, 0).add(
                {idx0, idx1}, std::move(cells_idxs), Count{_count_data(idx0, idx1)});
        }

        CellsIdxs& cells_idxs()
        {
            return _cells_idxs;
        }

        void set_stream_class(
            StreamClass const stream_class)
        {
            _stream_class = stream_class;
        }

    private:

        CellsIdxs& _cells_idxs;

        InterPartitionStreamData& _stream_data;

        StreamClass _stream_class;

        PartitionIOData& _partition_io_data;

        CountData _count_data;

};

}  // namespace inter_partition_stream


template<
    typename Policies,
    typename InterPartitionStreamPartition,
    typename PartitionIOPartition,
    typename FlowDirectionPartitions>
hpx::tuple<InterPartitionStreamPartition, PartitionIOPartition> inter_partition_stream_partition(
    Policies const& policies,
    OffsetT<PartitionIOPartition> const& partition_offset,
    FlowDirectionPartitions const& flow_direction_partitions)
{
    using Offset = OffsetT<PartitionT<FlowDirectionPartitions>>;
    hpx::future<Offset> offset{flow_direction_partitions(1, 1).offset()};

    // Determine location of input cells ---------------------------------------

    // Determine shapes of all partitions
    auto const partition_shapes = detail::partition_shapes(flow_direction_partitions);

    // Request the minimum amount of required data from all partitions
    auto flow_direction_data = partition_data(flow_direction_partitions, partition_shapes);

    using FlowDirectionPartition = PartitionT<FlowDirectionPartitions>;
    using FlowDirectionData = DataT<FlowDirectionPartition>;
    using FlowDirectionElement = ElementT<FlowDirectionPartitions>;

    using InflowCountElement = std::uint8_t;
    using InflowCountPartition = PartitionT<FlowDirectionPartition, InflowCountElement>;
    using InflowCountData = DataT<InflowCountPartition>;

    using InterPartitionStreamData = DataT<InterPartitionStreamPartition>;
    using StreamClass = ElementT<InterPartitionStreamData>;

    using PartitionIOData = DataT<PartitionIOPartition>;

    using Accumulator = inter_partition_stream::Accumulator<InterPartitionStreamData, PartitionIOData>;

    // Once the data from all partitions have arrived...
    return hpx::split_future(hpx::dataflow(
        hpx::launch::async,
        hpx::util::unwrapping(

                [policies, partition_offset](
                    Offset offset,
                    lue::Array<FlowDirectionData, rank<FlowDirectionData>> const& flow_direction_data)
                {
                    // Calculate inflow_count

                    // Output policy type for CountElement. Base it
                    // on the output policy for the stream classification
                    // passed in.
                    using InflowCountPolicy = policy::OutputPoliciesT<Policies, 0, InflowCountElement>;

                    // Input policy for FlowDirectionElement. This is
                    // the same one as passed in.
                    using FlowDirectionPolicy = policy::InputPoliciesT<Policies, 0>;

                    using InflowCountPolicies =
                        policy::Policies<
                            policy::AllValuesWithinDomain<FlowDirectionElement>,
                            policy::OutputsPolicies<InflowCountPolicy>,
                            policy::InputsPolicies<FlowDirectionPolicy>>;
                    InflowCountPolicies inflow_count_policies{
                        policy::AllValuesWithinDomain<FlowDirectionElement>{},
                        InflowCountPolicy{}, std::get<0>(policies.inputs_policies())};

                    auto const& [inflow_count_data, input_cell_idxs] =
                        detail::inflow_count_data<InflowCountPolicies, InflowCountData>(
                            inflow_count_policies, flow_direction_data);
                    lue_assert(std::is_sorted(input_cell_idxs.begin(), input_cell_idxs.end()));

                    auto const partition_shape{flow_direction_data(1, 1).shape()};

                    InterPartitionStreamData inter_partition_stream_data{partition_shape};
                    PartitionIOData partition_io_data{{1, 1}};

                    // Below, partition input cell idxs are accumulated
                    // through the flow direction network. We need an
                    // array of collections of cell idxs.
                    Array<std::vector<std::array<Index, 2>>, 2> cells_idxs{partition_shape};

                    auto const [nr_elements0, nr_elements1] = partition_shape;

                    // First iterate over all ridge cells. At each such cell:
                    // - Iterate downstream while updating the inflow count
                    // - Stop when a cell is reached with inflow count >
                    //     1, or the partition border
                    // After this step, all intra-partition stream cells have
                    // been visited and marked. The remaining cells are
                    // part of the inter-partition stream network.

                    // When accumulating values, the inflow count will be
                    // changed. We need to be able to make a distinction
                    // between ridge cells (inflow count is zero) and
                    // visited cells (inflow count >= zero). We therefore
                    // make a copy of the original inflow counts here.
                    auto inflow_count_data_copy{deep_copy(inflow_count_data)};
                    Accumulator accumulator{cells_idxs, inter_partition_stream_data, partition_io_data};

                    StreamClass const intra_partition_stream{0};
                    StreamClass const inter_partition_stream{1};

                    accumulator.set_stream_class(intra_partition_stream);

                    auto const& indp_inflow_count =
                        std::get<0>(inflow_count_policies.outputs_policies()).output_no_data_policy();
                    auto const& ondp_stream = std::get<0>(policies.outputs_policies()).output_no_data_policy();

                    for(Index idx0 = 0; idx0 < nr_elements0; ++idx0) {
                        for(Index idx1 = 0; idx1 < nr_elements1; ++idx1)
                        {
                            // Ridge cells are cells that don't receive
                            // input from other cells
                            if(indp_inflow_count.is_no_data(inflow_count_data, idx0, idx1))
                            {
                                ondp_stream.mark_no_data(inter_partition_stream_data, idx0, idx1);
                            }
                            else if(inflow_count_data(idx0, idx1) == 0)
                            {
                                // Material is an empty collection of
                                // partition input cell idxs.
                                // Push this collection downstream
                                // until a cell is reached with inflow
                                // count > 1.
                                inter_partition_stream_data(idx0, idx1) = intra_partition_stream;
                                accumulate(accumulator, idx0, idx1,
                                    flow_direction_data(1, 1), inflow_count_data_copy);
                            }
                        }
                    }

                    // Now iterate over all partition input cells and push
                    // the collection of input cell idxs to the corresponding
                    // partition output cell, or a sink. We need to know:
                    // - The indices of all partition output cells
                    // - The indices of zero or more partition input
                    //     cells linked to each output cell
                    // - Which cells are part of the inter-partition
                    //     stream network

                    // Iterate over all input cell idxs. Note that a
                    // single input cell can receive inputs from multiple
                    // neighbouring partition cells. In that case
                    // the input cell index occurs multiple times in the
                    // collection of cell indices. The inflow count will be
                    // larger than 1 in that case. But since inflow count
                    // also counts inflow from within the partition, the
                    // inflow count may be larger that the number
                    // of times a specific input cell index occurs in
                    // input_cell_idxs. It will never be smaller.
                    {
                        Index count;

                        accumulator.set_stream_class(inter_partition_stream);

                        for(auto it = input_cell_idxs.begin(); it != input_cell_idxs.end(); ++it)
                        {
                            // Figure out how many cells in neighbouring
                            // partitions flow into this input cell
                            count = 1;
                            while(it + 1 != input_cell_idxs.end() && *(it + 1) == *it)
                            {
                                ++count;
                                ++it;
                            }

                            // Material is a collection of count copies of
                            // index *it. Push this collection downstream
                            // until a cell is reached with inflow count > 1.

                            // Insert indices to collection at partition
                            // input cell
                            auto const [idx0, idx1] = *it;
                            accumulator.cells_idxs()(idx0, idx1).insert(
                                cells_idxs(idx0, idx1).begin(), count, *it);

                            // Mark this cell as part of an
                            // inter-partition stream
                            inter_partition_stream_data(idx0, idx1) = inter_partition_stream;

                            // Update the inflow counts
                            lue_assert(inflow_count_data_copy(idx0, idx1) >= count);
                            inflow_count_data_copy(idx0, idx1) -= count;

                            // If the remaining inflow count is
                            // larger than zero, then there is another
                            // stream originating in a partition input
                            // cell passing through this cell. In that
                            // case we can leave the accumulation on
                            // the latter cell.
                            if(inflow_count_data_copy(idx0, idx1) == 0)
                            {
                                accumulate(accumulator, idx0, idx1,
                                    flow_direction_data(1, 1), inflow_count_data_copy);
                            }
                        }
                    }

                    // Done. All inflow counts must be zero now.
                    /*
                    // TODO assert this
                    {
                        equal_to_policy → base on inflow count policy
                        all_policy → base on equal_to_policy
                        lue_assert(all(all_policy, equal_to(equal_to_policy, inflow_count_data_copy, 0)).get());
                    }
                    */

                    return hpx::make_tuple(
                        InterPartitionStreamPartition{
                            hpx::find_here(), offset, std::move(inter_partition_stream_data)},
                        PartitionIOPartition{
                            hpx::find_here(), partition_offset, std::move(partition_io_data)});
                }

            ),
        std::move(offset),
        when_all_get(std::move(flow_direction_data))));
}


template<
    typename Policies,
    typename InterPartitionStreamPartition,
    typename PartitionIOPartition,
    typename FlowDirectionPartitions>
struct InterPartitionStreamPartitionAction:
    hpx::actions::make_action<
        decltype(&inter_partition_stream_partition<
            Policies, InterPartitionStreamPartition, PartitionIOPartition, FlowDirectionPartitions>),
        &inter_partition_stream_partition<
            Policies, InterPartitionStreamPartition, PartitionIOPartition, FlowDirectionPartitions>,
        InterPartitionStreamPartitionAction<
            Policies, InterPartitionStreamPartition, PartitionIOPartition, FlowDirectionPartitions>>::type
{};

}  // namespace detail


namespace policy::inter_partition_stream {

    template<
        typename BooleanElement,
        typename FlowDirectionElement>
    using DefaultPolicies = policy::DefaultPolicies<
        AllValuesWithinDomain<FlowDirectionElement>,
        OutputElements<BooleanElement>,
        InputElements<FlowDirectionElement>>;

}  // namespace policy::inter_partition_stream


template<
    typename Policies,
    typename BooleanElement,
    typename CountElement,  // Number of cells contributing to output cell
    typename FlowDirectionElement,
    Rank rank>
std::tuple<
    PartitionedArray<BooleanElement, rank>,
    PartitionedArray<ArrayPartitionIO<lue::Index, rank, CountElement>, rank>>
        inter_partition_stream(
    Policies const& policies,
    PartitionedArray<FlowDirectionElement, rank> const& flow_direction)
{
    using FlowDirectionArray = PartitionedArray<FlowDirectionElement, rank>;
    using FlowDirectionPartitions = PartitionsT<FlowDirectionArray>;

    using InterPartitionStreamArray = PartitionedArray<BooleanElement, rank>;
    using InterPartitionStreamPartitions = PartitionsT<InterPartitionStreamArray>;
    using InterPartitionStreamPartition = PartitionT<InterPartitionStreamArray>;

    using PartitionIOArray = PartitionedArray<ArrayPartitionIO<lue::Index, rank, CountElement>, rank>;
    using PartitionIOPartitions = PartitionsT<PartitionIOArray>;
    using PartitionIOPartition = PartitionT<PartitionIOArray>;

    lue_assert(all_are_valid(flow_direction.partitions()));

    Localities<rank> const& localities{flow_direction.localities()};
    FlowDirectionPartitions const& flow_direction_partitions{flow_direction.partitions()};

    // Create halo partitions with flow directions away from the array
    auto halo_corner_partitions =
        detail::halo_corner_partitions<FlowDirectionPartitions>(localities);
    auto halo_longitudinal_side_partitions =
        detail::halo_longitudinal_side_partitions<FlowDirectionPartitions>(
            localities, flow_direction_partitions);
    auto halo_latitudinal_side_partitions =
        detail::halo_latitudinal_side_partitions<FlowDirectionPartitions>(
            localities, flow_direction_partitions);

    lue_assert(all_are_valid(halo_corner_partitions));
    lue_assert(all_are_valid(halo_longitudinal_side_partitions));
    lue_assert(all_are_valid(halo_latitudinal_side_partitions));

    // Attach continuations to groups of nine partitions
    using Action = detail::InterPartitionStreamPartitionAction<
        Policies, InterPartitionStreamPartition, PartitionIOPartition, FlowDirectionPartitions>;

    auto [inter_partition_stream_partitions, partition_io_partitions] =
        detail::spawn_partitions<Policies, InterPartitionStreamPartitions, PartitionIOPartitions>(
            policies, Action{}, localities, flow_direction_partitions,
            halo_corner_partitions, halo_longitudinal_side_partitions, halo_latitudinal_side_partitions);

    lue_assert(all_are_valid(inter_partition_stream_partitions));
    lue_assert(all_are_valid(partition_io_partitions));

    return std::make_tuple(
        InterPartitionStreamArray{
            shape(flow_direction), localities, std::move(inter_partition_stream_partitions)},
        PartitionIOArray{
            shape(flow_direction.partitions()), localities, std::move(partition_io_partitions)});
}


template<
    typename BooleanElement,
    typename CountElement,
    typename FlowDirectionElement,
    Rank rank>
std::tuple<
    PartitionedArray<BooleanElement, rank>,
    PartitionedArray<ArrayPartitionIO<lue::Index, 2, CountElement>, rank>>
        inter_partition_stream(
    PartitionedArray<FlowDirectionElement, rank> const& flow_direction)
{
    using Policies = policy::inter_partition_stream::DefaultPolicies<BooleanElement, FlowDirectionElement>;

    return inter_partition_stream<Policies, BooleanElement, CountElement, FlowDirectionElement, rank>(
        Policies{}, flow_direction);
}

}  // namespace lue
