#pragma once
#include "lue/framework/algorithm/accu_threshold3.hpp"
#include "lue/framework/algorithm/definition/inflow_count3.hpp"
#include "lue/framework/algorithm/export.hpp"
#include "lue/macro.hpp"


namespace lue {
    namespace detail {

        template<
            typename MaterialElement,
            Rank rank>  // Remove parameter
        class ChannelMaterial
        {

            public:

                ChannelMaterial()=default;


                ChannelMaterial(
                    Index const& idx,
                    MaterialElement const& value):

                    _cell_idx{idx},
                    _value{value}

                {
                }


                Index const& cell_idx() const
                {
                    return _cell_idx;
                }


                MaterialElement const& value() const
                {
                    return _value;
                }


            private:

                friend class hpx::serialization::access;


                template<typename Archive>
                void serialize(
                    Archive& archive,
                    [[maybe_unused]] unsigned int const version)
                {
                    archive & _cell_idx & _value;
                }


                Index _cell_idx;

                MaterialElement _value;

        };


        template<
            typename MaterialElement,
            Rank rank>
        class MaterialCommunicator:
            public Communicator<ChannelMaterial<MaterialElement, rank>, rank>
        {

            public:

                using Base = Communicator<ChannelMaterial<MaterialElement, rank>, rank>;


                MaterialCommunicator()=default;


                MaterialCommunicator(
                    hpx::naming::id_type const locality_id,
                    std::string const& scope,
                    Shape<Count, rank> const& shape_in_partitions,
                    Indices<Index, rank> const& partition_idxs):

                    Base{locality_id, "/lue/" + scope, shape_in_partitions, partition_idxs}

                {
                }


            private:

                friend class hpx::serialization::access;


                template<typename Archive>
                void serialize(
                    Archive& archive,
                    unsigned int const version)
                {
                    Base::serialize(archive, version);
                }

        };


        template<
            typename InflowNoDataPolicy,
            typename ThresholdNoDataPolicy,
            typename OutflowNoDataPolicy,
            typename RemainderNoDataPolicy,
            Rank rank>
        class AccuThresholdCellAccumulator
        {

            public:

                using MaterialElement = policy::ElementT<InflowNoDataPolicy>;
                static_assert(std::is_same_v<policy::ElementT<InflowNoDataPolicy>, MaterialElement>);
                static_assert(std::is_same_v<policy::ElementT<ThresholdNoDataPolicy>, MaterialElement>);
                static_assert(std::is_same_v<policy::ElementT<OutflowNoDataPolicy>, MaterialElement>);
                static_assert(std::is_same_v<policy::ElementT<RemainderNoDataPolicy>, MaterialElement>);

                using MaterialData = DataT<PartitionedArray<MaterialElement, rank>>;

                AccuThresholdCellAccumulator(
                    InflowNoDataPolicy const& indp_inflow,
                    ThresholdNoDataPolicy const& indp_threshold,
                    OutflowNoDataPolicy const& ondp_outflow,
                    RemainderNoDataPolicy const& ondp_remainder,
                    MaterialData const& inflow,
                    MaterialData const& threshold,
                    MaterialData& outflow,
                    MaterialData& remainder):

                    _indp_inflow{indp_inflow},
                    _indp_threshold{indp_threshold},
                    _ondp_outflow{ondp_outflow},
                    _ondp_remainder{ondp_remainder},
                    _inflow{inflow},
                    _threshold{threshold},
                    _outflow{outflow},
                    _remainder{remainder}

                {
                }


                void accumulate_external_inflow(
                    Index const idx0,
                    Index const idx1)
                {
                    MaterialElement const& inflow{_inflow(idx0, idx1)};
                    MaterialElement const& threshold{_threshold(idx0, idx1)};

                    MaterialElement& outflow{_outflow(idx0, idx1)};
                    MaterialElement& remainder{_remainder(idx0, idx1)};

                    lue_hpx_assert(
                        (_ondp_outflow.is_no_data(outflow) && _ondp_remainder.is_no_data(remainder)) ||
                        (!_ondp_outflow.is_no_data(outflow) && !_ondp_remainder.is_no_data(remainder)));

                    if(!_ondp_outflow.is_no_data(outflow))
                    {
                        if(_indp_inflow.is_no_data(inflow) ||
                            _indp_threshold.is_no_data(threshold))
                        {
                            _ondp_outflow.mark_no_data(outflow);
                            _ondp_remainder.mark_no_data(remainder);
                        }
                        else
                        {
                            // TODO Domain check
                            lue_hpx_assert(threshold >= 0);

                            // Now we know the final, total amount
                            // of inflow that enters this cell
                            outflow += inflow;

                            // Split this amount into outflow and remainder,
                            // based on the threshold passed in

                            MaterialElement mobile_material{outflow - threshold};

                            if(mobile_material > 0)
                            {
                                remainder = threshold;
                                outflow = mobile_material;
                            }
                            else
                            {
                                remainder = outflow;
                                outflow = 0;
                            }
                        }
                    }
                }


                void accumulate_downstream(
                    Index const idx0_from,
                    Index const idx1_from,
                    Index const idx0_to,
                    Index const idx1_to)
                {
                    // The results for the upstream cell are ready. Use
                    // its outflow as inflow for the downstream cell.
                    MaterialElement const& outflow{_outflow(idx0_from, idx1_from)};
                    MaterialElement const& upstream_remainder{_remainder(idx0_from, idx1_from)};

                    MaterialElement& inflow{_outflow(idx0_to, idx1_to)};
                    MaterialElement& remainder{_remainder(idx0_to, idx1_to)};

                    if(!_ondp_outflow.is_no_data(inflow))
                    {
                        lue_hpx_assert(!_ondp_remainder.is_no_data(remainder));

                        if(_ondp_outflow.is_no_data(outflow))
                        {
                            lue_hpx_assert(_ondp_remainder.is_no_data(upstream_remainder));

                            _ondp_outflow.mark_no_data(inflow);
                            _ondp_remainder.mark_no_data(remainder);
                        }
                        else
                        {
                            // Just add the outflow from upstream to
                            // the inflow of the downstream cell
                            inflow += outflow;
                        }
                    }
                }


                void accumulate_downstream(
                    MaterialElement const& outflow,
                    Index const idx0_to,
                    Index const idx1_to)
                {
                    // The results for the upstream cell are ready
                    MaterialElement& inflow{_outflow(idx0_to, idx1_to)};
                    MaterialElement& remainder{_remainder(idx0_to, idx1_to)};

                    if(!_ondp_outflow.is_no_data(inflow))
                    {
                        lue_hpx_assert(!_ondp_remainder.is_no_data(remainder));

                        if(_ondp_outflow.is_no_data(outflow))
                        {
                            _ondp_outflow.mark_no_data(inflow);
                            _ondp_remainder.mark_no_data(remainder);
                        }
                        else
                        {
                            // Just add the outflow from upstream to
                            // the inflow of the downstream cell
                            inflow += outflow;
                        }
                    }
                }


                MaterialElement const& outflow(
                    Index const idx0,
                    Index const idx1) const
                {
                    return _outflow(idx0, idx1);
                }


            private:

                InflowNoDataPolicy _indp_inflow;

                ThresholdNoDataPolicy _indp_threshold;

                OutflowNoDataPolicy _ondp_outflow;

                RemainderNoDataPolicy _ondp_remainder;

                MaterialData const& _inflow;  // External inflow

                MaterialData const& _threshold;

                MaterialData& _outflow;  // Upstream inflow, outflow

                MaterialData& _remainder;

        };


        template<
             typename CellAccumulator,
             typename Communicator
        >
        class Accumulator
        {

            public:

                using MaterialElement = typename CellAccumulator::MaterialElement;


                Accumulator(
                    CellAccumulator&& cell_accumulator,
                    Communicator& communicator):

                    _cell_accumulator{std::forward<CellAccumulator>(cell_accumulator)},
                    _communicator{communicator}

                {
                }


                void enter_cell(
                    Index const idx0,
                    Index const idx1)
                {
                    // What to do when we enter a cell
                    this->accumulate_external_inflow(idx0, idx1);
                }


                void leave_at_sink(
                    [[maybe_unused]] Index const idx0,
                    [[maybe_unused]] Index const idx1)
                {
                    // What to do when we leave at a sink
                }


                void leave_at_output_cell(
                    Count const extent0,
                    Count const extent1,
                    Index const idx0,
                    Index const idx1,
                    Index const offset0,
                    Index const offset1)
                {
                    // What to do when we leave at a partition output cell
                    this->leave_partition(extent0, extent1, idx0, idx1, offset0, offset1);
                }


                void push_downstream(
                    Index const idx0_from,
                    Index const idx1_from,
                    Index const idx0_to,
                    Index const idx1_to)
                {
                    // What to do when we have to push material downstream
                    this->accumulate_downstream(idx0_from, idx1_from, idx0_to, idx1_to);
                }


                void enter_at_partition_input(
                    MaterialElement const& value,
                    Index const idx0_to,
                    Index const idx1_to)
                {
                    _cell_accumulator.accumulate_downstream(value, idx0_to, idx1_to);
                }


            private:


                void accumulate_external_inflow(
                    Index const idx0,
                    Index const idx1)
                {
                    // We are entering the current cell. Material from
                    // upstream is already present.

                    // Add external input material for the current cell to
                    // the material already present.
                    // This can happen unconditionally.
                    _cell_accumulator.accumulate_external_inflow(idx0, idx1);
                }


                void accumulate_downstream(
                    Index const idx0_from,
                    Index const idx1_from,
                    Index const idx0_to,
                    Index const idx1_to)
                {
                    // We are about the leave the current cell. This is the
                    // final step.

                    // Aggregate material in the current cell with the
                    // downstream one.
                    // How much material is aggregated, depends on the
                    // accumulation operation, e.g.: all of it, a fraction,
                    // everything above some threshold, ...
                    _cell_accumulator.accumulate_downstream(idx0_from, idx1_from, idx0_to, idx1_to);
                }


                void leave_partition(
                    Count const extent0,
                    Count const extent1,
                    Index const idx0,
                    Index const idx1,
                    Index const offset0,
                    Index const offset1)
                {
                    // Send material to cell in neighbouring partition
                    auto [direction, idx] = destination_cell(extent0, extent1, idx0, idx1, offset0, offset1);

                    if(_communicator.has_neighbour(direction))
                    {
                        // We are not at the border of the array
                        using Value = typename Communicator::Value;

                        _communicator.send(direction, Value{idx, _cell_accumulator.outflow(idx0, idx1)});
                    }
                }


            private:

                CellAccumulator _cell_accumulator;

                Communicator& _communicator;

        };


        enum class AccumulationExitCellClass: std::uint8_t
        {
            sink,
            partition_output,
            junction,
        };


        template<
            typename Accumulator,
            typename Index,
            typename FlowDirectionData,
            typename InflowCountData>
        std::tuple<std::array<Index, 2>, AccumulationExitCellClass> accumulate(
            Accumulator& accumulator,
            Index idx0,
            Index idx1,
            FlowDirectionData const& flow_direction_data,
            InflowCountData& inflow_count_data)
        {
            auto const [nr_elements0, nr_elements1] = flow_direction_data.shape();
            Index offset0, offset1;
            bool is_within_partition;
            AccumulationExitCellClass cell_class;

            while(true)
            {
                lue_hpx_assert(inflow_count_data(idx0, idx1) == 0);

                // We are about to leave the current cell. All material from
                // upstream (if any) has been accumulated in the cell already.
                // We still need to add the external input.
                accumulator.enter_cell(idx0, idx1);

                // Determine what to do next
                is_within_partition = downstream_cell(
                    flow_direction_data, nr_elements0, nr_elements1, idx0, idx1, offset0, offset1);

                // First check conditions that will end the current stream ---------
                if(offset0 == 0 && offset1 == 0)
                {
                    // Current cell is a sink. This is the end of this
                    // stream.
                    accumulator.leave_at_sink(idx0, idx1);
                    cell_class = AccumulationExitCellClass::sink;
                    break;
                }

                // Downstream cell is pointed to by idx0 + offset0 and idx1 + offset1
                if(!is_within_partition)
                {
                    // Current cell is partition output cell. This is the
                    // end of this stream in the current partition. Finish
                    // calculations an exit.
                    accumulator.leave_at_output_cell(nr_elements0, nr_elements1, idx0, idx1, offset0, offset1);
                    cell_class = AccumulationExitCellClass::partition_output;
                    break;
                }

                // Stream continues within this partition --------------------------

                // Push material to the downstream cell
                accumulator.push_downstream(idx0, idx1, idx0 + offset0, idx1 + offset1);

                // Prepare for next iteration
                idx0 += offset0;
                idx1 += offset1;

                // Update the inflow count of the downstream cell
                lue_hpx_assert(inflow_count_data(idx0, idx1) >= 1);
                --inflow_count_data(idx0, idx1);

                if(inflow_count_data(idx0, idx1) > 0)
                {
                    // There are other streams flowing into the new / downstream cell → stop
                    cell_class = AccumulationExitCellClass::junction;
                    break;
                }
            }

            return std::make_tuple(std::array<Index, 2>{idx0, idx1}, cell_class);
        }


        template<
            typename MaterialElement,
            typename IdxConverter,
            typename Accumulate,
            Rank rank>
        void monitor_material_inputs(
            std::vector<std::array<Index, rank>>&& input_cells_idxs,
            typename MaterialCommunicator<MaterialElement, rank>::Channel&& channel,
            IdxConverter&& idx_to_idxs,
            Accumulate accumulate)
        {
            lue_hpx_assert(channel);

            // Whenever material arrives in the channel, call the
            // accumulator to accumulate it through the partition

            // The number of times material should arrive in the channel
            // is equal to the number of input cells at the specific side
            // of the partition.

            for(auto const& material: channel)
            {
                auto const cell_idxs{idx_to_idxs(material.cell_idx())};

                auto it = std::find_if(input_cells_idxs.begin(), input_cells_idxs.end(),
                        [cell_idxs1=cell_idxs](std::array<Index, rank> const& cell_idxs2)
                        {
                            return cell_idxs1 == cell_idxs2;
                        }
                    );
                lue_hpx_assert(it != input_cells_idxs.end());
                input_cells_idxs.erase(it);

                accumulate(cell_idxs, material.value());

                if(input_cells_idxs.empty())
                {
                    break;
                }
            }
        }


        template<
            typename Policies,
            typename FlowDirectionElement,
            typename MaterialElement,
            Rank rank>
        hpx::tuple<ArrayPartition<MaterialElement, rank>, ArrayPartition<MaterialElement, rank>>
            accu_threshold3_partition(
                Policies const& policies,
                ArrayPartition<FlowDirectionElement, rank> const& flow_direction_partition,
                ArrayPartition<MaterialElement, rank> const& external_inflow_partition,
                ArrayPartition<MaterialElement, rank> const& threshold_partition,
                InflowCountCommunicator<rank> inflow_count_communicator,
                MaterialCommunicator<MaterialElement, rank> material_communicator)
        {
            using FlowDirectionPartition = ArrayPartition<FlowDirectionElement, rank>;
            using FlowDirectionData = DataT<FlowDirectionPartition>;
            using MaterialPartition = ArrayPartition<MaterialElement, rank>;
            using MaterialData = DataT<MaterialPartition>;
            using Offset = OffsetT<FlowDirectionPartition>;

            using CountElement = std::uint8_t;
            using InflowCountPartition = ArrayPartition<CountElement, rank>;
            using InflowCountData = DataT<InflowCountPartition>;
            using CellsIdxs = std::vector<std::array<Index, rank>>;

            // Determine inflow count
            InflowCountPartition inflow_count_partition{};
            hpx::shared_future<std::array<CellsIdxs, nr_neighbours<rank>()>> input_cells_idxs_f{};
            hpx::future<CellsIdxs> output_cells_idxs_f{};
            {
                // As long as we only use flow direction, external inflow
                // and threshold to detect no-data in input, there is no
                // need to mark no-data in the output of inflow_count. We
                // won't be reading these cells anyway.
                using InflowCountOutputPolicies =
                    policy::OutputPolicies<policy::DontMarkNoData<CountElement>>;
                InflowCountOutputPolicies inflow_count_output_policies{};

                using FlowDirectionInputPolicies = policy::InputPoliciesT<Policies, 0>;
                FlowDirectionInputPolicies flow_direction_input_policies{
                    std::get<0>(policies.inputs_policies())};

                using InflowCountPolicies =
                    policy::Policies<
                        policy::AllValuesWithinDomain<FlowDirectionElement>,
                        policy::OutputsPolicies<InflowCountOutputPolicies>,
                        policy::InputsPolicies<FlowDirectionInputPolicies>>;

                InflowCountPolicies inflow_count_policies{
                    policy::AllValuesWithinDomain<FlowDirectionElement>{},
                    inflow_count_output_policies,
                    flow_direction_input_policies};

                hpx::tie(inflow_count_partition, input_cells_idxs_f, output_cells_idxs_f) =
                    inflow_count3_action<CountElement>(
                        inflow_count_policies, flow_direction_partition,
                        std::move(inflow_count_communicator));
            }

            hpx::future<Offset> partition_offset_f = hpx::dataflow(
                    hpx::launch::async,

                    [](FlowDirectionPartition const& flow_direction_partition)
                    {
                        return flow_direction_partition.offset();
                    },

                    flow_direction_partition
                );


            // Solve intra-partition stream cells
            hpx::future<MaterialData> outflow_data_f;
            hpx::future<MaterialData> remainder_data_f;
            hpx::future<InflowCountData> inflow_count_data_f;
            {
                // Once the input partitions and inflow_count_data
                // are ready, spawn a task that will calculate the flow
                // accumulation for the intra-partition stream
                // cells. Whenever material reaches the border of the
                // partition, it is sent to the corresponding task
                // managing the neighbouring partition.
                hpx::tie(outflow_data_f, remainder_data_f, inflow_count_data_f) =
                    hpx::split_future(hpx::dataflow(
                            hpx::launch::async,

                                [policies, material_communicator](
                                    FlowDirectionPartition const& flow_direction_partition,
                                    MaterialPartition const& external_inflow_partition,
                                    MaterialPartition const& threshold_partition,
                                    InflowCountPartition const& inflow_count_partition) mutable
                                {
                                    auto const flow_direction_partition_ptr{
                                        ready_component_ptr(flow_direction_partition)};
                                    FlowDirectionData const& flow_direction_data{
                                        flow_direction_partition_ptr->data()};

                                    auto const external_inflow_partition_ptr{
                                        ready_component_ptr(external_inflow_partition)};
                                    MaterialData const& external_inflow_data{
                                        external_inflow_partition_ptr->data()};

                                    auto const threshold_partition_ptr{
                                        ready_component_ptr(threshold_partition)};
                                    MaterialData const& threshold_data{
                                        threshold_partition_ptr->data()};

                                    auto const inflow_count_partition_ptr{
                                        ready_component_ptr(inflow_count_partition)};
                                    InflowCountData const& inflow_count_data{
                                        inflow_count_partition_ptr->data()};

                                    auto const& partition_shape{inflow_count_data.shape()};

                                    DataT<MaterialPartition> outflow_data{partition_shape, 0};
                                    DataT<MaterialPartition> remainder_data{partition_shape, 0};

                                    auto const [nr_elements0, nr_elements1] = partition_shape;

                                    auto const& indp_flow_direction =
                                        std::get<0>(policies.inputs_policies()).input_no_data_policy();
                                    auto const& indp_inflow =
                                        std::get<1>(policies.inputs_policies()).input_no_data_policy();
                                    auto const& indp_threshold =
                                        std::get<2>(policies.inputs_policies()).input_no_data_policy();
                                    auto const& ondp_outflow =
                                        std::get<0>(policies.outputs_policies()).output_no_data_policy();
                                    auto const& ondp_remainder =
                                        std::get<1>(policies.outputs_policies()).output_no_data_policy();

                                    // We need to copy inflow counts:
                                    // - inflow counts are used to select ridge cells
                                    // - downstream accumulation updates inflow counts
                                    InflowCountData inflow_count_data_copy{deep_copy(inflow_count_data)};

                                    using CellAccumulator = AccuThresholdCellAccumulator<
                                        decltype(indp_inflow), decltype(indp_threshold),
                                        decltype(ondp_outflow), decltype(ondp_remainder), rank>;
                                    using Communicator = MaterialCommunicator<MaterialElement, rank>;

                                    CellAccumulator cell_accumulator{
                                        indp_inflow, indp_threshold, ondp_outflow, ondp_remainder,
                                        external_inflow_data, threshold_data, outflow_data, remainder_data};
                                    Accumulator<CellAccumulator, Communicator> accumulator{
                                        std::move(cell_accumulator), material_communicator};

                                    for(Index idx0 = 0; idx0 < nr_elements0; ++idx0) {
                                        for(Index idx1 = 0; idx1 < nr_elements1; ++idx1)
                                        {
                                            if(indp_flow_direction.is_no_data(flow_direction_data, idx0, idx1))
                                            {
                                                // Skip cells for which we don't have a flow-direction
                                                ondp_outflow.mark_no_data(outflow_data, idx0, idx1);
                                                ondp_remainder.mark_no_data(remainder_data, idx0, idx1);
                                            }
                                            else if(inflow_count_data(idx0, idx1) == 0)
                                            {
                                                accumulate(
                                                    accumulator, idx0, idx1, flow_direction_data,
                                                    inflow_count_data_copy);
                                            }
                                        }
                                    }

                                    return hpx::make_tuple(
                                        std::move(outflow_data), std::move(remainder_data),
                                        std::move(inflow_count_data_copy));
                                },

                            flow_direction_partition,
                            external_inflow_partition,
                            threshold_partition,
                            inflow_count_partition
                        ));
            }

            // Solve inter-partition stream cells
            {
                // Once the futures of the results of the
                // intra-partition stream calculations have become
                // ready, spawn a task per neighbouring receive channel,
                // each of which must monitor incoming material values
                // and perform flow accumulation calculation once they
                // do. Each of these tasks must stop once all relevant
                // partition input cells (given the direction of the
                // neighbour), have been received.
                hpx::tie(outflow_data_f, remainder_data_f) = hpx::split_future(hpx::dataflow(
                        hpx::launch::async,

                            [policies, material_communicator=std::move(material_communicator)](
                                FlowDirectionPartition const& flow_direction_partition,
                                MaterialPartition const& external_inflow_partition,
                                MaterialPartition const& threshold_partition,
                                hpx::future<InflowCountData>&& inflow_count_data_f,
                                hpx::shared_future<std::array<CellsIdxs, nr_neighbours<rank>()>> const& input_cells_idxs_f,
                                hpx::future<MaterialData>&& outflow_data_f,
                                hpx::future<MaterialData>&& remainder_data_f) mutable
                            {
                                std::vector<accu::Direction> const directions{
                                    accu::Direction::north,
                                    accu::Direction::north_east,
                                    accu::Direction::east,
                                    accu::Direction::south_east,
                                    accu::Direction::south,
                                    accu::Direction::south_west,
                                    accu::Direction::west,
                                    accu::Direction::north_west,
                                };

                                // The accumulate function must wrap this call:
                                //     accumulate(
                                //         accumulator, idx0, idx1, flow_direction_data,
                                //         inflow_count_data_copy);
                                // It must also synchronize access
                                // to the accumulate call so no two
                                // threads call it at the same time
                                //
                                // Calling the function must
                                // - lock access to the data
                                // - accumulate material
                                // - release the lock

                                auto const flow_direction_partition_ptr{
                                    ready_component_ptr(flow_direction_partition)};
                                FlowDirectionData const& flow_direction_data{
                                    flow_direction_partition_ptr->data()};
                                auto const& partition_shape{flow_direction_data.shape()};

                                auto const external_inflow_partition_ptr{
                                    ready_component_ptr(external_inflow_partition)};
                                MaterialData const& external_inflow_data{
                                    external_inflow_partition_ptr->data()};

                                auto const threshold_partition_ptr{
                                    ready_component_ptr(threshold_partition)};
                                MaterialData const& threshold_data{
                                    threshold_partition_ptr->data()};

                                InflowCountData inflow_count_data{inflow_count_data_f.get()};
                                MaterialData outflow_data{outflow_data_f.get()};
                                MaterialData remainder_data{remainder_data_f.get()};

                                auto const& indp_inflow =
                                    std::get<1>(policies.inputs_policies()).input_no_data_policy();
                                auto const& indp_threshold =
                                    std::get<2>(policies.inputs_policies()).input_no_data_policy();
                                auto const& ondp_outflow =
                                    std::get<0>(policies.outputs_policies()).output_no_data_policy();
                                auto const& ondp_remainder =
                                    std::get<1>(policies.outputs_policies()).output_no_data_policy();

                                using CellAccumulator = AccuThresholdCellAccumulator<
                                    decltype(indp_inflow), decltype(indp_threshold),
                                    decltype(ondp_outflow), decltype(ondp_remainder), rank>;
                                using Communicator = MaterialCommunicator<MaterialElement, rank>;

                                CellAccumulator cell_accumulator{
                                    indp_inflow, indp_threshold, ondp_outflow, ondp_remainder,
                                    external_inflow_data, threshold_data, outflow_data, remainder_data};
                                Accumulator<CellAccumulator, Communicator> accumulator{
                                    std::move(cell_accumulator), material_communicator};

                                hpx::lcos::local::mutex accu_mutex;

                                auto accumulate =
                                    [
                                        &accu_mutex,
                                        &accumulator,
                                        &flow_direction_data,
                                        &inflow_count_data
                                    ](
                                        std::array<Index, rank> const& cell_idxs,
                                        MaterialElement const value) mutable
                                    {
                                        auto [idx0, idx1] = cell_idxs;

                                        // Prevent multiple threads
                                        // from touching this data at the
                                        // same time
                                        std::scoped_lock lock{accu_mutex};

                                        lue_hpx_assert(inflow_count_data(idx0, idx1) >= 1);

                                        accumulator.enter_at_partition_input(value, idx0, idx1);

                                        --inflow_count_data(idx0, idx1);

                                        // Note that multiple streams
                                        // from other partitions can join
                                        // in a single partition input cell. Only
                                        // start an accumulation if this is
                                        // the last one.
                                        if(inflow_count_data(idx0, idx1) == 0)
                                        {
                                            detail::accumulate(
                                                accumulator, idx0, idx1, flow_direction_data,
                                                inflow_count_data);
                                        }
                                    };
                                using Accumulate = decltype(accumulate);

                                std::array<CellsIdxs, nr_neighbours<rank>()> input_cells_idxs{
                                    input_cells_idxs_f.get()};

                                std::vector<hpx::future<void>> results{};
                                results.reserve(nr_neighbours<rank>());

                                auto const [extent0, extent1] = partition_shape;

                                {
                                    CellsIdxs cells_idxs{input_cells_idxs[accu::Direction::north]};

                                    if(!cells_idxs.empty())
                                    {
                                        RowIdxConverter north_idx_converter{};
                                        results.push_back(hpx::async(
                                            monitor_material_inputs<
                                                MaterialElement, decltype(north_idx_converter),
                                                Accumulate, rank>,
                                            std::move(cells_idxs),
                                            material_communicator.receive_channel(accu::Direction::north),
                                            north_idx_converter, accumulate));
                                    }
                                }

                                {
                                    CellsIdxs cells_idxs{input_cells_idxs[accu::Direction::south]};

                                    if(!cells_idxs.empty())
                                    {
                                        RowIdxConverter south_idx_converter{extent0 - 1};
                                        results.push_back(hpx::async(
                                            monitor_material_inputs<
                                                MaterialElement, decltype(south_idx_converter),
                                                Accumulate, rank>,
                                            std::move(cells_idxs),
                                            material_communicator.receive_channel(accu::Direction::south),
                                            south_idx_converter, accumulate));
                                    }
                                }

                                {
                                    CellsIdxs cells_idxs{input_cells_idxs[accu::Direction::west]};

                                    if(!cells_idxs.empty())
                                    {
                                        ColIdxConverter west_idx_converter{};
                                        results.push_back(hpx::async(
                                            monitor_material_inputs<
                                                MaterialElement, decltype(west_idx_converter),
                                                Accumulate, rank>,
                                            std::move(cells_idxs),
                                            material_communicator.receive_channel(accu::Direction::west),
                                            west_idx_converter, accumulate));
                                    }
                                }

                                {
                                    CellsIdxs cells_idxs{input_cells_idxs[accu::Direction::east]};

                                    if(!cells_idxs.empty())
                                    {
                                        ColIdxConverter east_idx_converter{extent1 - 1};
                                        results.push_back(hpx::async(
                                            monitor_material_inputs<
                                                MaterialElement, decltype(east_idx_converter),
                                                Accumulate, rank>,
                                            std::move(cells_idxs),
                                            material_communicator.receive_channel(accu::Direction::east),
                                            east_idx_converter, accumulate));
                                    }
                                }

                                {
                                    CellsIdxs cells_idxs{input_cells_idxs[accu::Direction::north_west]};

                                    if(!cells_idxs.empty())
                                    {
                                        CornerIdxConverter north_west_idx_converter{};
                                        results.push_back(hpx::async(
                                            monitor_material_inputs<
                                                MaterialElement, decltype(north_west_idx_converter),
                                                Accumulate, rank>,
                                            std::move(cells_idxs),
                                            material_communicator.receive_channel(accu::Direction::north_west),
                                            north_west_idx_converter, accumulate));
                                    }
                                }

                                {
                                    CellsIdxs cells_idxs{input_cells_idxs[accu::Direction::north_east]};

                                    if(!cells_idxs.empty())
                                    {
                                        CornerIdxConverter north_east_idx_converter{0, extent1 - 1};
                                        results.push_back(hpx::async(
                                            monitor_material_inputs<
                                                MaterialElement, decltype(north_east_idx_converter),
                                                Accumulate, rank>,
                                            std::move(cells_idxs),
                                            material_communicator.receive_channel(accu::Direction::north_east),
                                            north_east_idx_converter, accumulate));
                                    }
                                }

                                {
                                    CellsIdxs cells_idxs{input_cells_idxs[accu::Direction::south_east]};

                                    if(!cells_idxs.empty())
                                    {
                                        CornerIdxConverter south_east_idx_converter{extent0 - 1, extent1 - 1};
                                        results.push_back(hpx::async(
                                            monitor_material_inputs<
                                                MaterialElement, decltype(south_east_idx_converter),
                                                Accumulate, rank>,
                                            std::move(cells_idxs),
                                            material_communicator.receive_channel(accu::Direction::south_east),
                                            south_east_idx_converter, accumulate));
                                    }
                                }

                                {
                                    CellsIdxs cells_idxs{input_cells_idxs[accu::Direction::south_west]};

                                    if(!cells_idxs.empty())
                                    {
                                        CornerIdxConverter south_west_idx_converter{extent0 - 1, 0};
                                        results.push_back(hpx::async(
                                            monitor_material_inputs<
                                                MaterialElement, decltype(south_west_idx_converter),
                                                Accumulate, rank>,
                                            std::move(cells_idxs),
                                            material_communicator.receive_channel(accu::Direction::south_west),
                                            south_west_idx_converter, accumulate));
                                    }
                                }

                                // As a side effect of this wait,
                                // the partitions whose data is used in
                                // the above monitor_material threads
                                // do not go out of scope
                                hpx::wait_all(results);
                                lue_hpx_assert(all_are_valid(results));
                                lue_hpx_assert(all_are_ready(results));

                                return hpx::make_tuple(std::move(outflow_data), std::move(remainder_data));
                            },


                        flow_direction_partition,
                        external_inflow_partition,
                        threshold_partition,
                        inflow_count_data_f,
                        input_cells_idxs_f,
                        outflow_data_f,
                        remainder_data_f
                    ));
            }

            // Once the futures of the results of the
            // inter-partition stream calculations have become ready,
            // return the result partitions.
            auto [outflow_partition_f, remainder_partition_f] = hpx::split_future(hpx::dataflow(
                    hpx::launch::async,
                    hpx::util::unwrapping(

                            [](
                                Offset&& partition_offset,
                                MaterialData&& outflow_data,
                                MaterialData&& remainder_data)
                            {
                                using Server = typename MaterialPartition::Server;

                                return hpx::make_tuple(
                                    MaterialPartition{
                                        hpx::new_<Server>(hpx::find_here(),
                                            partition_offset, std::move(outflow_data))},
                                    MaterialPartition{
                                        hpx::new_<Server>(hpx::find_here(),
                                            partition_offset, std::move(remainder_data))});
                            }

                        ),

                    std::move(partition_offset_f),
                    std::move(outflow_data_f),
                    std::move(remainder_data_f)
                ));

            return hpx::make_tuple(
                MaterialPartition{std::move(outflow_partition_f)},
                MaterialPartition{std::move(remainder_partition_f)});
        }


        template<
            typename Policies,
            typename FlowDirectionElement,
            typename MaterialElement,
            Rank rank>
        struct AccuThresholdAction3:
            hpx::actions::make_action<
                decltype(&accu_threshold3_partition<
                    Policies, FlowDirectionElement, MaterialElement, rank>),
                &accu_threshold3_partition<
                    Policies, FlowDirectionElement, MaterialElement, rank>,
                AccuThresholdAction3<
                    Policies, FlowDirectionElement, MaterialElement, rank>>::type
        {};

    }  // namespace detail


    template<
        typename Policies,
        typename FlowDirectionElement,
        typename MaterialElement,
        Rank rank>
    std::tuple<
        PartitionedArray<MaterialElement, rank>,
        PartitionedArray<MaterialElement, rank>>
            accu_threshold3(
                Policies const& policies,
                PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
                PartitionedArray<MaterialElement, rank> const& external_inflow,
                PartitionedArray<MaterialElement, rank> const& threshold)
    {
        // This function must:
        // - Return as soon as possible
        // - Create a task per partition, returning partitions per output
        // - Create communicators for use by the tasks
        // - Create a scope for the communicators, keeping them
        //   alive for as long as there are tasks using them

        using MaterialArray = PartitionedArray<MaterialElement, rank>;
        using MaterialPartitions = PartitionsT<MaterialArray>;

        auto const& shape_in_partitions{flow_direction.partitions().shape()};
        Localities<rank> const& localities{flow_direction.localities()};

        // Create communicators used in solving the flow accumulation
        using InflowCountCommunicator = detail::InflowCountCommunicator<rank>;
        using InflowCountCommunicatorArray = detail::CommunicatorArray<InflowCountCommunicator, rank>;
        using MaterialCommunicator = detail::MaterialCommunicator<MaterialElement, rank>;
        using MaterialCommunicatorArray = detail::CommunicatorArray<MaterialCommunicator, rank>;

        static std::size_t invocation_id{0};
        InflowCountCommunicatorArray inflow_count_communicators{
            "accu_threshold3/inflow_count/" + std::to_string(invocation_id), localities};
        MaterialCommunicatorArray material_communicators{
            "accu_threshold3/" + std::to_string(invocation_id), localities};
        ++invocation_id;

        // For each partition, spawn a task that will solve the
        // flow accumulation for the partition.
        MaterialPartitions outflow_partitions{shape_in_partitions};
        MaterialPartitions remainder_partitions{shape_in_partitions};
        detail::AccuThresholdAction3<Policies, FlowDirectionElement, MaterialElement, rank> action{};
        Count const nr_partitions{nr_elements(shape_in_partitions)};

        for(Index p = 0; p < nr_partitions; ++p)
        {
            hpx::tie(outflow_partitions[p], remainder_partitions[p]) = hpx::split_future(hpx::async(
                action, localities[p], policies,
                flow_direction.partitions()[p],
                external_inflow.partitions()[p], threshold.partitions()[p],
                inflow_count_communicators[p], material_communicators[p]));
        }

        // The communicators used by the tasks calculating the flow
        // accumulation result must be kept alive until these results
        // are ready. We do this by attaching a continuation,
        // passing in the communicators. Once the results are ready,
        // these communicators go out of scope. Note that component
        // clients are reference-counted.
        hpx::when_all(outflow_partitions.begin(), outflow_partitions.end(),
            [
                communicators1=std::move(inflow_count_communicators),
                communicators2=std::move(material_communicators)
            ]([[maybe_unused]] auto&& partitions)
            {
                HPX_UNUSED(communicators1);
                HPX_UNUSED(communicators2);
            });

        return std::make_tuple(
            MaterialArray{flow_direction.shape(), localities, std::move(outflow_partitions)},
            MaterialArray{flow_direction.shape(), localities, std::move(remainder_partitions)});
    }

}  // namespace lue


#define LUE_INSTANTIATE_ACCU_THRESHOLD3(                                              \
    Policies, FlowDirectionElement, MaterialElement)                                  \
                                                                                      \
    template LUE_FA_EXPORT                                                            \
    std::tuple<                                                                       \
        PartitionedArray<MaterialElement, 2>,                                         \
        PartitionedArray<MaterialElement, 2>> accu_threshold3<                        \
            ArgumentType<void(Policies)>, FlowDirectionElement, MaterialElement, 2>(  \
        ArgumentType<void(Policies)> const&,                                          \
        PartitionedArray<FlowDirectionElement, 2> const&,                             \
        PartitionedArray<MaterialElement, 2> const&,                                  \
        PartitionedArray<MaterialElement, 2> const&);
