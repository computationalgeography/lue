#pragma once
#include "lue/framework/algorithm/component/array_partition_io.hpp"
#include "lue/framework/algorithm/detail/flow_direction.hpp"
#include "lue/framework/algorithm/detail/when_all_get.hpp"
#include "lue/framework/core/array.hpp"
#include "lue/framework/core/assert.hpp"
#include "lue/framework/core/offset.hpp"
#include <vector>


namespace lue::detail {

    template<
        typename Index,
         Rank rank>
    Offset<Index, rank> invert(
        Offset<Index, rank> const& offset)
        {
            Offset<Index, rank> result{offset};

            result[0] *= -1;
            result[1] *= -1;

            return result;
        };


    template<
        typename PartitionIOComponents>
    Array<hpx::future<bool>, rank<PartitionIOComponents>> solved_partitions(
        PartitionIOComponents& partition_io_partitions,
        Array<bool, 2> const& ready_partitions)
    {
        // For each partition that is not yet ready, request whether
        // it is solved. If a partition is solved, then it is potentially
        // drainable from its output cells, to a downstream partition.
        // In theory, a solved partition is not drainable. In that
        // case it contains sinks that receive all material.
        using PartitionIOComponent = ComponentT<PartitionIOComponents>;
        using Shape = ShapeT<PartitionIOComponents>;

        Rank const rank{lue::rank<PartitionIOComponents>};
        Shape const& shape_in_partitions{partition_io_partitions.shape()};
        Count const nr_partitions{nr_elements(shape_in_partitions)};

        // Only ask partitions that are not already ready, whether
        // they are solved.
        Array<hpx::future<bool>, rank> is_solved{shape_in_partitions};

        for(Index p = 0; p < nr_partitions; ++p)
        {
            if(ready_partitions[p])
            {
                is_solved[p] = hpx::make_ready_future<bool>(true);
            }
            else
            {
                is_solved[p] = partition_io_partitions[p].then(

                        [](PartitionIOComponent const& partition_io_partition)
                        {
                            return partition_io_partition.is_solved();
                        }

                    );
            }
        }

        return is_solved;
    }


    template<
        typename PartitionIOComponents>
    hpx::future<Array<
        typename ComponentT<PartitionIOComponents>::PartitionOffsets,
        rank<PartitionIOComponents>>>
            upstream_partition_offsets(
                PartitionIOComponents& partition_io_partitions,
                Array<bool, rank<PartitionIOComponents>> const& ready_partitions)
    {
        using PartitionIOComponent = ComponentT<PartitionIOComponents>;
        using PartitionOffsets = typename PartitionIOComponent::PartitionOffsets;
        using Shape = ShapeT<PartitionIOComponents>;

        Rank const rank{lue::rank<PartitionIOComponents>};
        Shape const& shape_in_partitions{partition_io_partitions.shape()};
        Count const nr_partitions{nr_elements(shape_in_partitions)};

        Array<hpx::future<PartitionOffsets>, rank> downstream_partition_offsets{shape_in_partitions};

        for(Index p = 0; p < nr_partitions; ++p)
        {
            if(!ready_partitions[p])
            {
                downstream_partition_offsets[p] = partition_io_partitions[p].partition_offsets();
            }
            else
            {
                downstream_partition_offsets[p] = hpx::make_ready_future<PartitionOffsets>();
            }
        }

        // Attach a continuation to each collection of futures to
        // offsets and create an array with per partition a set
        // of offsets to partitions that have material ready for us.

        // Halo partitions are not relevant anymore. Halo partitions
        // will never produce material for non-halo partitions.

        // Non-halo partitions can produce material for
        // halo-partitions, but that is irrelevant. Skip'm.

        return when_all_get(std::move(downstream_partition_offsets)).then(
                [](
                    hpx::future<Array<PartitionOffsets, rank>>&& downstream_partition_offsets_f)
                {
                    // For each partition,
                    // downstream_partition_offsets contains the
                    // offsets *to* the downstream partition it
                    // can provide material for. We need to convert
                    // this to an array with for each partition the
                    // offsets to the upstream partitions material
                    // can be provided *from*.

                    using Offset = typename PartitionIOComponent::Offset;

                    Array<PartitionOffsets, rank> downstream_partition_offsets{
                        downstream_partition_offsets_f.get()};
                    Shape const& shape_in_partitions{downstream_partition_offsets.shape()};
                    Array<PartitionOffsets, rank> upstream_partition_offsets{shape_in_partitions};

                    auto const [extent0, extent1] = shape_in_partitions;

                    auto flows_towards_halo_partition =
                        [extent0, extent1](
                            Index const idx0,
                            Index const idx1,
                            Offset const& offset) -> bool
                        {
                            return
                                (idx0 == 0 && offset[0] == -1) ||
                                (idx0 == extent0 - 1 && offset[0] == 1) ||
                                (idx1 == 0 && offset[1] == -1) ||
                                (idx1 == extent1 - 1 && offset[1] == 1);
                        };

                    for(Index idx0 = 0; idx0 < extent0; ++idx0)
                    {
                        for(Index idx1 = 0; idx1 < extent1; ++idx1)
                        {
                            for(auto const& offset: downstream_partition_offsets(idx0, idx1))
                            {
                                if(!flows_towards_halo_partition(idx0, idx1, offset))
                                {
                                    upstream_partition_offsets(idx0 + offset[0], idx1 + offset[1]).insert(invert(offset));
                                }
                            }
                        }
                    }

                    return upstream_partition_offsets;
                }

            );
    }


    template<
        typename PartitionIOComponents>
    std::tuple<
            Array<bool, 2>,
            Array<std::set<Offset<std::int8_t, 2>>, 2>
        >
            flow_accumulation_front(
                PartitionIOComponents& partition_io_partitions,
                Array<bool, 2> const& ready_partitions)
    {
        Rank const rank{lue::rank<PartitionIOComponents>};

        // For each partition that is not ready yet, determine
        // whether it is fully solved.
        Array<bool, rank> solved_partitions_{partition_io_partitions.shape()};
        {
            Array<hpx::future<bool>, rank> solved_partitions_f{
                solved_partitions(partition_io_partitions, ready_partitions)};
            hpx::wait_all(solved_partitions_f.begin(), solved_partitions_f.end());
            std::transform(solved_partitions_f.begin(), solved_partitions_f.end(),
                solved_partitions_.begin(), [](auto& f) { return f.get(); });
        }

        // For each partition that is not ready yet, determine the
        // offsets to upstream solved partitions.
        using PartitionIOComponent = ComponentT<PartitionIOComponents>;
        using PartitionOffsets = typename PartitionIOComponent::PartitionOffsets;

        hpx::future<Array<PartitionOffsets, rank>> upstream_partition_offsets_f{
            upstream_partition_offsets(partition_io_partitions, ready_partitions)};
        upstream_partition_offsets_f.wait();

        return std::make_tuple(std::move(solved_partitions_), upstream_partition_offsets_f.get());
    }


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


            void enter_cell(
                Index const idx0,
                Index const idx1)
            {
                this->accumulate_input(idx0, idx1);
            }


            void leave_at_sink(
                [[maybe_unused]] Index const idx0,
                [[maybe_unused]] Index const idx1)
            {
            }


            void leave_at_output_cell(
                Index const idx0,
                Index const idx1,
                Index const offset0,
                Index const offset1)
            {
                this->leave_partition(idx0, idx1, offset0, offset1);
            }


            void move_downstream(
                Index const idx0_from,
                Index const idx1_from,
                Index const idx0_to,
                Index const idx1_to)
            {
                this->accumulate_downstream(idx0_from, idx1_from, idx0_to, idx1_to);
            }


            // TODO private
            // TODO rename to grab_external_material
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


            // TODO make private
            // TODO rename to push_material_downstream
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


            // TODO make private
            // TODO rename to cache_result
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
                break;
            }

            // Downstream cell is pointed to by idx0 + offset0 and idx1 + offset1
            if(!is_within_partition)
            {
                // Current cell is partition output cell. This is the
                // end of this stream in the current partition. Finish
                // calculations an exit.
                accumulator.leave_at_output_cell(idx0, idx1, offset0, offset1);
                break;
            }

            // Stream continues within this partition --------------------------

            // Move material to the downstream cell
            accumulator.move_downstream(idx0, idx1, idx0 + offset0, idx1 + offset1);

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
