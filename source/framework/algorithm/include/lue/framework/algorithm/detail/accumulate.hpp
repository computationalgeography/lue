#pragma once
#include "lue/framework/algorithm/detail/flow_direction.hpp"
#include "lue/framework/core/assert.hpp"
#include <vector>


namespace lue::detail {

    template<
        typename PartitionIOData_,
        typename InputMaterialData_,
        typename CellAccumulator,
        typename OutputMaterialData_=InputMaterialData_>
    class Accumulator

    {

        public:

            using PartitionIOData = PartitionIOData_;
            using InputMaterialData = InputMaterialData_;
            using OutputMaterialData = OutputMaterialData_;
            using OutputMaterialElement = ElementT<OutputMaterialData>;

            static constexpr Rank rank{lue::rank<InputMaterialData>};


            Accumulator(
                PartitionIOData& partition_io_data,
                InputMaterialData const& input_material_data,
                CellAccumulator&& cell_accumulator,
                OutputMaterialData& output_material_data):

                _partition_io_data{partition_io_data},
                _input_material_data{input_material_data},
                _cell_accumulator{std::forward<CellAccumulator>(cell_accumulator)},
                _output_material_data{output_material_data}

            {
            }


            void accumulate_input(
                Index const idx0,
                Index const idx1)
            {
                // We are entering the current cell. Material from
                // upstream is already present.

                // Add external input material for the current cell to
                // the material already present.
                // This can happen unconditionally.
                _cell_accumulator.accumulate_input(
                    _input_material_data(idx0, idx1), _output_material_data(idx0, idx1));
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

                _cell_accumulator.accumulate_downstream(
                    _output_material_data(idx0_from, idx1_from), _output_material_data(idx0_to, idx1_to));
            }


            void leave_partition(
                Index const idx0,
                Index const idx1,
                Index const offset0,
                Index const offset1)
            {
                // The offset used in ArrayPartitionIO is narrower than
                // Index. We have to cast.
                using O = typename PartitionIOData::Offset::value_type;

                _partition_io_data.add_output_cell(
                    {idx0, idx1}, {static_cast<O>(offset0), static_cast<O>(offset1)},
                    OutputMaterialElement{_output_material_data(idx0, idx1)});
            }


            CellAccumulator const& cell_accumulator() const
            {
                return _cell_accumulator;
            }


        private:

            PartitionIOData& _partition_io_data;

            InputMaterialData const& _input_material_data;

            CellAccumulator _cell_accumulator;

            OutputMaterialData& _output_material_data;

    };


    template<
        typename Accumulator,
        typename Index,
        typename FlowDirectionData,
        typename InflowCountData>
    void accumulate(
        Accumulator& accumulator,
        Index idx0,
        Index idx1,
        FlowDirectionData const& flow_direction_data,
        InflowCountData& inflow_count_data)
    {
        auto const [nr_elements0, nr_elements1] = flow_direction_data.shape();
        Index offset0, offset1;
        bool is_within_partition;

        // The 'flow' in the loop below is:
        // - We enter a 'current' cell
        //     - This should result in input material being grabbed for
        //         this current location, by the accumulator
        // - We determine the downstream cell
        // - If downstream cell is outside of the partition, we leave
        //     the partition

        while(true)
        {
            lue_hpx_assert(inflow_count_data(idx0, idx1) == 0);

            // We are about to leave the current cell
            accumulator.accumulate_input(idx0, idx1);

            // Decide what to do next
            is_within_partition = downstream_cell(
                flow_direction_data, nr_elements0, nr_elements1, idx0, idx1, offset0, offset1);

            if(offset0 == 0 && offset1 == 0)
            {
                // Current cell is a sink → stop
                break;
            }

            // Downstream cell is pointed to by idx0 + offset0 and idx1 + offset1
            if(!is_within_partition)
            {
                // Current cell is partition output cell → notify accumulator and stop
                accumulator.leave_partition(idx0, idx1, offset0, offset1);
                break;
            }

            // All is well. Continuing within this partition. Go downstream.

            // Move material to the downstream cell
            accumulator.accumulate_downstream(idx0, idx1, idx0 + offset0, idx1 + offset1);

            // Prepare for next iteration
            idx0 += offset0;
            idx1 += offset1;

            // Update the inflow count of the downstream cell
            lue_hpx_assert(inflow_count_data(idx0, idx1) >= 1);
            --inflow_count_data(idx0, idx1);

            if(inflow_count_data(idx0, idx1) > 0)
            {
                // There are other streams flowing into the new cell → stop
                break;
            }
        }
    }

}  // namespace lue::detail
