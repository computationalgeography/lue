#pragma once
#include "lue/framework/algorithm/detail/accumulation_exit_cell_class.hpp"
#include "lue/framework/algorithm/detail/communicator.hpp"
#include "lue/framework/algorithm/definition/inflow_count3.hpp"  // destination_cell
#include "lue/framework/core/annotate.hpp"
#include "lue/framework/core/define.hpp"
#include <hpx/serialization.hpp>


namespace lue::detail {

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
                std::string const& basename,
                Shape<Count, rank> const& shape_in_partitions,
                Indices<Index, rank> const& partition_idxs):

                Base{locality_id, basename, shape_in_partitions, partition_idxs}

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
         typename CellAccumulator,
         typename Communicator
    >
    class Accumulator3
    {

        public:

            using MaterialElement = typename CellAccumulator::MaterialElement;


            Accumulator3(
                CellAccumulator&& cell_accumulator,
                Communicator& communicator,
                std::array<std::vector<std::array<Index, 2>>, 8>& output_cells_idxs):

                _cell_accumulator{std::forward<CellAccumulator>(cell_accumulator)},
                _communicator{communicator},
                _output_cells_idxs{output_cells_idxs}

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
                auto [direction, idx] = destination_cell(extent0, extent1, idx0, idx1, offset0, offset1);


                // Remove the partition output cell from the collection. Once all output cells
                // are solved / handled, the sending channel can be closed. This will end
                // the reading for loop on the other side of the channel.
                auto& output_cells_idxs{_output_cells_idxs[direction]};
                auto it = std::find_if(output_cells_idxs.begin(), output_cells_idxs.end(),
                        [idx0, idx1](std::array<Index, 2> const& cell_idxs2)
                        {
                            return idx0 == cell_idxs2[0] && idx1 == cell_idxs2[1];
                        }
                    );
                lue_hpx_assert(it != output_cells_idxs.end());
                output_cells_idxs.erase(it);


                // Send material to cell in neighbouring partition
                if(_communicator.has_neighbour(direction))
                {
                    // We are not at the border of the array
                    using Value = typename Communicator::Value;

                    _communicator.send(direction, Value{idx, _cell_accumulator.outflow(idx0, idx1)});

                    // The sending channel can be closed
                    if(output_cells_idxs.empty())
                    {
                        _communicator.close(direction);
                    }
                }
            }


        private:

            CellAccumulator _cell_accumulator;

            Communicator& _communicator;

            std::array<std::vector<std::array<Index, 2>>, 8>& _output_cells_idxs;

    };


    template<
        typename Accumulator,
        typename Index,
        typename FlowDirectionData,
        typename InflowCountData>
    std::tuple<std::array<Index, 2>, AccumulationExitCellClass> accumulate3(
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
        AnnotateFunction annotation{"inter_partition_stream"};
        lue_hpx_assert(channel);
        lue_hpx_assert(!input_cells_idxs.empty());

        // Whenever material arrives in the channel, call the
        // accumulator to accumulate it through the partition

        // The number of times material should arrive in the channel
        // is equal to the number of input cells at the specific side
        // of the partition.

        for(auto const& material: channel)
        {
            lue_hpx_assert(!input_cells_idxs.empty());

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
                // No material should be sent trough this channel
                // again. We don't need it. It would be a bug.
                break;
            }
        }

        lue_hpx_assert(input_cells_idxs.empty());
    }

}  // namespace lue::detail
