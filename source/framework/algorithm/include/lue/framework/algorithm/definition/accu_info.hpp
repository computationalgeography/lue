#pragma once
#include "lue/framework/algorithm/accu_info.hpp"
#include "lue/framework/algorithm/definition/accu.hpp"


namespace lue {
    namespace detail::accu_info {

        class InputMaterial
        {

            public:

                using InputMaterialElement = CellClass;
                using InputElement = std::tuple<InputMaterialElement const&>;


                InputMaterial():

                    _cell_class{undefined_cell}

                {
                }


                InputMaterial(CellClass const cell_class):

                    _cell_class{cell_class}

                {
                    lue_hpx_assert(
                        _cell_class == intra_partition_stream_cell ||
                        _cell_class == inter_partition_stream_cell);
                }


                InputElement operator()(
                    [[maybe_unused]] Index const idx0, [[maybe_unused]] Index const idx1) const
                {
                    return std::forward_as_tuple(_cell_class);
                }


            private:

                CellClass _cell_class;
        };


        template<typename CellClassArray>
        class OutputMaterial
        {

            public:

                using CellClassElement = ElementT<CellClassArray>;

                using MaterialElement = std::tuple<CellClassElement&>;

                using Element = std::tuple<CellClassElement>;


                OutputMaterial(CellClassArray& cell_class):

                    _cell_class{cell_class}

                {
                }


                MaterialElement operator()(Index const idx0, Index const idx1)
                {
                    return std::forward_as_tuple(_cell_class(idx0, idx1));
                }


            private:

                CellClassArray& _cell_class;
        };


        template<typename OutputCellClassNoDataPolicy>
        class CellAccumulator
        {

            public:

                using CellClassElement = policy::ElementT<OutputCellClassNoDataPolicy>;
                using InputElement = typename InputMaterial::InputElement;
                using OutputElement = std::tuple<CellClassElement&>;
                using ConstOutputElement = std::tuple<CellClassElement const&>;


                CellAccumulator(OutputCellClassNoDataPolicy const& ondp_cell_class):

                    _ondp_cell_class{ondp_cell_class}

                {
                }


                void accumulate_input(
                    [[maybe_unused]] InputElement const& input_element,
                    [[maybe_unused]] OutputElement output_element) const
                {
                    // There is no external input, yet

                    // if(!_ondp_cell_class.is_no_data(output_element))
                    // {
                    //     if(_indp_material.is_no_data(input_element))
                    //     {
                    //         _ondp_cell_class.mark_no_data(output_element);
                    //     }
                    //     else
                    //     {
                    //         output_element += input_element;
                    //     }
                    // }
                }


                void accumulate_downstream(
                    ConstOutputElement const& upstream_material_element,
                    OutputElement downstream_material_element) const
                {
                    auto const& [upstream_cell_class] = upstream_material_element;
                    auto& [downstream_cell_class] = downstream_material_element;

                    if (!_ondp_cell_class.is_no_data(downstream_cell_class))
                    {
                        if (_ondp_cell_class.is_no_data(upstream_cell_class))
                        {
                            _ondp_cell_class.mark_no_data(downstream_cell_class);
                        }
                        else
                        {
                            // When handling the intra-partition stream
                            // cells, this will push the intra-partition
                            // stream class code into inter-partition
                            // stream cells as well. But when handling
                            // the inter-partition stream cells, the
                            // correct code will be pushed in, overwriting
                            // the intra-partition stream cell class code.
                            downstream_cell_class = upstream_cell_class;
                        }
                    }
                }


            private:

                OutputCellClassNoDataPolicy _ondp_cell_class;
        };


        template<typename Index, Rank rank>
        class Accumulator
        {

            public:

                using InputMaterial = accu_info::InputMaterial;

                template<typename CellClassData>
                using OutputMaterial = accu_info::OutputMaterial<CellClassData>;

                using Material = std::tuple<CellClass>;

                template<typename OutputCellClassNoDataPolicy>
                using CellAccumulator = accu_info::CellAccumulator<OutputCellClassNoDataPolicy>;
        };


        template<
            typename Policies,
            typename CellClassPartition,
            typename PartitionIOComponent,
            typename InflowCountPartition,
            typename FlowDirectionPartitions,
            typename Accumulator>
        hpx::tuple<CellClassPartition, PartitionIOComponent, InflowCountPartition>
        solve_intra_partition_stream_cells(
            Policies const& policies,
            OffsetT<PartitionT<FlowDirectionPartitions>> const& partition_offset,
            FlowDirectionPartitions const& flow_direction_partitions)
        {
            using FlowDirectionPartition = PartitionT<FlowDirectionPartitions>;
            using FlowDirectionData = DataT<FlowDirectionPartition>;
            using FlowDirectionElement = ElementT<FlowDirectionPartitions>;

            using CellClassData = DataT<CellClassPartition>;

            using PartitionIOData = typename PartitionIOComponent::Data;
            using InflowCountData = DataT<InflowCountPartition>;
            using InflowCountElement = ElementT<InflowCountPartition>;

            using Offset = OffsetT<FlowDirectionPartition>;

            hpx::future<Offset> offset{flow_direction_partitions(1, 1).offset()};

            // Determine shapes of all partitions
            auto const partition_shapes = detail::partition_shapes(flow_direction_partitions);

            // Request the minimum amount of required data from all partitions
            auto flow_direction_data = partition_data(flow_direction_partitions, partition_shapes);
            // auto input_material_data = partition_data(material_partitions, partition_shapes);

            // Once all input data has arrived...
            return hpx::split_future(hpx::dataflow(
                hpx::launch::async,
                hpx::unwrapping(


                    [policies, partition_offset](
                        Offset offset,
                        lue::Array<FlowDirectionData, rank<FlowDirectionData>> const& flow_direction_data)
                    {
                        // -------------------------------------------------
                        // Determine inflow_count
                        // As long as we only use flow_direction
                        // and material to detect no-data in input,
                        // there is no need to mark no-data in output
                        // of inflow_count
                        using InflowCountOutputPolicies = policy::OutputPolicies<
                            policy::DontMarkNoData<InflowCountElement>,
                            policy::AllValuesWithinRange<InflowCountElement, FlowDirectionElement>>;
                        InflowCountOutputPolicies inflow_count_output_policies{};

                        using FlowDirectionInputPolicies = policy::InputPoliciesT<Policies, 0>;
                        FlowDirectionInputPolicies flow_direction_input_policies{
                            std::get<0>(policies.inputs_policies())};

                        using InflowCountPolicies = policy::Policies<
                            policy::AllValuesWithinDomain<FlowDirectionElement>,
                            policy::OutputsPolicies<InflowCountOutputPolicies>,
                            policy::InputsPolicies<FlowDirectionInputPolicies>>;

                        InflowCountPolicies inflow_count_policies{
                            policy::AllValuesWithinDomain<FlowDirectionElement>{},
                            inflow_count_output_policies,
                            flow_direction_input_policies};

                        auto [inflow_count_data, input_cell_idxs] =
                            detail::inflow_count_data<InflowCountData>(
                                inflow_count_policies, flow_direction_data);
                        lue_hpx_assert(std::is_sorted(input_cell_idxs.begin(), input_cell_idxs.end()));


                        // -------------------------------------------------
                        // Determine PartitionIO and calculate flow
                        // accumulation for intra-partition cells
                        auto const& indp_flow_direction =
                            std::get<0>(policies.inputs_policies()).input_no_data_policy();
                        // auto const& indp_material =
                        //     std::get<1>(policies.inputs_policies()).input_no_data_policy();
                        auto const& ondp_cell_class =
                            std::get<0>(policies.outputs_policies()).output_no_data_policy();

                        using CellAccumulator =
                            typename Accumulator::template CellAccumulator<decltype(ondp_cell_class)>;

                        using Shape = ShapeT<InflowCountData>;
                        Shape const partition_shape{inflow_count_data.shape()};

                        PartitionIOData partition_io_data{partition_shape, std::move(input_cell_idxs)};
                        CellClassData cell_class_data{partition_shape, undefined_cell};

                        using InputMaterial = typename Accumulator::InputMaterial;
                        using OutputMaterial = typename Accumulator::template OutputMaterial<CellClassData>;
                        using Accumulator2 = detail::
                            Accumulator<PartitionIOData, InputMaterial, CellAccumulator, OutputMaterial>;

                        InputMaterial input_material;  // {input_cell_idxs};
                        OutputMaterial output_material{cell_class_data};
                        Accumulator2 accumulator{
                            partition_io_data,
                            input_material,
                            CellAccumulator{ondp_cell_class},
                            output_material};

                        auto const [nr_elements0, nr_elements1] = partition_shape;

                        InflowCountData inflow_count_data_copy{deep_copy(inflow_count_data)};

                        for (Index idx0 = 0; idx0 < nr_elements0; ++idx0)
                        {
                            for (Index idx1 = 0; idx1 < nr_elements1; ++idx1)
                            {
                                if (indp_flow_direction.is_no_data(flow_direction_data(1, 1), idx0, idx1))
                                {
                                    // Skip cells for which we don't have a flow-direction
                                    ondp_cell_class.mark_no_data(cell_class_data, idx0, idx1);
                                }
                                else if (inflow_count_data(idx0, idx1) == 0)
                                {
                                    cell_class_data(idx0, idx1) = intra_partition_stream_cell;

                                    detail::accumulate(
                                        accumulator,
                                        idx0,
                                        idx1,
                                        flow_direction_data(1, 1),
                                        inflow_count_data_copy);

                                    cell_class_data(idx0, idx1) = ridge_cell;
                                }
                            }
                        }


                        // -------------------------------------------------
                        // Return all results
                        return hpx::make_tuple(
                            CellClassPartition{hpx::find_here(), offset, std::move(cell_class_data)},
                            PartitionIOComponent{hpx::find_here(), std::move(partition_io_data)},
                            InflowCountPartition{
                                hpx::find_here(), offset, std::move(inflow_count_data_copy)});
                    }

                    ),
                std::move(offset),
                when_all_get(std::move(flow_direction_data))));
        }


        template<
            typename Policies,
            typename FlowDirectionPartition,
            typename PartitionIOComponent,
            typename InflowCountPartition,
            typename CellClassPartition,
            typename Accumulator>
        void accu_info_inter_partition_stream_cells(
            Policies const& policies,
            FlowDirectionPartition const& flow_direction_partition,
            InflowCountPartition const& inflow_count_partition,
            Array<PartitionIOComponent, 2> io_components,
            typename PartitionIOComponent::PartitionOffsets const& offsets,
            /// CellClassPartition const& material_partition,
            CellClassPartition const& cell_class_partition)
        {
            /// lue_hpx_assert(material_partition.is_ready());

            using CellClassData = DataT<CellClassPartition>;
            using Offset = typename PartitionIOComponent::Offset;
            using Shape = ShapeT<InflowCountPartition>;

            lue_hpx_assert(!offsets.empty());

            // All inputs are ready. Just grab the server instances of the
            // components which are not used by others at the same
            // time(!), and get on with it.

            auto flow_direction_partition_ptr{detail::ready_component_ptr(flow_direction_partition)};
            auto& flow_direction_partition_server{*flow_direction_partition_ptr};
            auto flow_direction_data{flow_direction_partition_server.data()};

            auto inflow_count_partition_ptr{detail::ready_component_ptr(inflow_count_partition)};
            auto& inflow_count_partition_server{*inflow_count_partition_ptr};
            auto inflow_count_data{inflow_count_partition_server.data()};

            // auto material_partition_ptr{detail::ready_component_ptr(material_partition)};
            // auto& material_partition_server{*material_partition_ptr};
            // auto material_data{material_partition_server.data()};

            auto cell_class_partition_ptr{detail::ready_component_ptr(cell_class_partition)};
            auto& cell_class_partition_server{*cell_class_partition_ptr};
            auto cell_class_data{cell_class_partition_server.data()};

            Shape const& partition_shape{flow_direction_partition_server.shape()};

            using Indices = typename PartitionIOComponent::Indices;
            using MaterialElement = typename PartitionIOComponent::Value;

            std::vector<hpx::future<std::vector<std::tuple<Indices, MaterialElement>>>> input_cells_f;
            input_cells_f.reserve(offsets.size());

            // Iterate over the neighbouring partitions that have material
            // for us.
            for (Offset const& offset : offsets)
            {
                PartitionIOComponent& io_component{io_components(1 + offset[0], 1 + offset[1])};

                // Request the material from neighbouring I/O
                // components. Note that a neighbouring partition can
                // provide material from multiple output cells.
                input_cells_f.push_back(io_component.drain(invert(offset), partition_shape));
            }

            hpx::wait_all(input_cells_f.begin(), input_cells_f.end());

            // Grab the values from the futures
            std::vector<std::vector<std::tuple<Indices, MaterialElement>>> input_cells_;
            input_cells_.resize(input_cells_f.size());

            std::transform(
                input_cells_f.begin(),
                input_cells_f.end(),
                input_cells_.begin(),
                [](auto& future) { return future.get(); });

            // Neighbours have drained. Now we need to push the material
            // we received through our partition.

            auto partition_io_ptr{detail::ready_component_ptr(io_components(1, 1))};
            auto& partition_io_server{*partition_io_ptr};
            auto& partition_io_data{partition_io_server.data()};

            // auto const& indp_material =
            //     std::get<1>(policies.inputs_policies()).input_no_data_policy();
            auto const& ondp_cell_class = std::get<0>(policies.outputs_policies()).output_no_data_policy();

            using CellAccumulator = typename Accumulator::template CellAccumulator<decltype(ondp_cell_class)>;

            using PartitionIOData = typename PartitionIOComponent::Data;

            using InputMaterial = typename Accumulator::InputMaterial;
            using OutputMaterial = typename Accumulator::template OutputMaterial<CellClassData>;
            using Accumulator2 =
                detail::Accumulator<PartitionIOData, InputMaterial, CellAccumulator, OutputMaterial>;

            InputMaterial input_material;
            OutputMaterial output_material{cell_class_data};
            Accumulator2 accumulator{
                partition_io_data, input_material, CellAccumulator{ondp_cell_class}, output_material};

            for (auto const& input_cells : input_cells_)
            {
                lue_hpx_assert(!input_cells.empty());

                for (auto const& [input_cell_idxs, material] : input_cells)
                {
                    // Insert material at input cell and
                    // propagate as far as we can

                    // In case the material is no-data, propagate this
                    // in downstream direction

                    auto const [idx0, idx1] = input_cell_idxs;

                    lue_hpx_assert(inflow_count_data(idx0, idx1) >= 1);

                    accumulator.cell_accumulator().accumulate_downstream(
                        material, output_material(idx0, idx1));

                    --inflow_count_data(idx0, idx1);
                    partition_io_data.remove_input_cell(input_cell_idxs);

                    if (inflow_count_data(idx0, idx1) == 0)
                    {
                        cell_class_data(idx0, idx1) = inter_partition_stream_cell;

                        // Push material downstream until a cell is
                        // reached with inflow count > 1.
                        auto [exit_cell_idxs, cell_class] = detail::accumulate(
                            accumulator, idx0, idx1, flow_direction_data, inflow_count_data);

                        cell_class_data(idx0, idx1) = partition_input_cell;

                        if (cell_class == AccumulationExitCellClass::partition_output)
                        {
                            cell_class_data(exit_cell_idxs) = partition_output_cell;
                        }
                    }
                }
            }
        }


        template<
            typename Policies,
            typename FlowDirectionPartition,
            typename PartitionIOComponent,
            typename InflowCountPartition,
            typename CellClassPartition,
            typename Accumulator>
        struct AccuInfoInterPartitionStreamCellsAction:
            hpx::actions::make_action<
                decltype(&accu_info_inter_partition_stream_cells<
                         Policies,
                         FlowDirectionPartition,
                         PartitionIOComponent,
                         InflowCountPartition,
                         CellClassPartition,
                         Accumulator>),
                &accu_info_inter_partition_stream_cells<
                    Policies,
                    FlowDirectionPartition,
                    PartitionIOComponent,
                    InflowCountPartition,
                    CellClassPartition,
                    Accumulator>,
                AccuInfoInterPartitionStreamCellsAction<
                    Policies,
                    FlowDirectionPartition,
                    PartitionIOComponent,
                    InflowCountPartition,
                    CellClassPartition,
                    Accumulator>>::type
        {
        };


        template<
            typename Policies,
            typename Localities,
            typename FlowDirectionPartitions,
            typename PartitionIOComponents,
            typename InflowCountPartitions,
            typename CellClassPartitions,
            typename Accumulator>
        void solve_inter_partition_stream_cells(
            Policies const& policies,
            Localities const& localities,
            FlowDirectionPartitions const& flow_direction_partitions,
            PartitionIOComponents&& partition_io_partitions,
            InflowCountPartitions&& inflow_count_partitions,
            CellClassPartitions&& cell_class_partitions,
            IDPromiseArray<rank<CellClassPartitions>>&& cell_class_promises,
            hpx::promise<std::vector<PartitionedArray<PartitionClass, 2>>>&& partition_class_arrays_promise,
            hpx::promise<std::vector<PartitionedArray<double, 2>>>&& solvable_fraction_arrays_promise,
            hpx::promise<std::vector<PartitionedArray<std::uint32_t, 2>>>&& nr_cells_to_solve_arrays_promise)
        {
            // Perform as many iterations as needed to calculate a global
            // flow accumulation solution

            // As soon as a partition is ready, set the corresponding
            // promise's ID. This will allow any continuations to be
            // scheduled for execution, even though not all partitions
            // calculated here are ready yet.

            using FlowDirectionPartition = ComponentT<FlowDirectionPartitions>;
            using PartitionIOComponent = ComponentT<PartitionIOComponents>;
            using InflowCountPartition = ComponentT<InflowCountPartitions>;
            using CellClassPartition = ComponentT<CellClassPartitions>;

            using PartitionClassArray = PartitionedArray<PartitionClass, 2>;
            using PartitionClassPartitions = PartitionsT<PartitionClassArray>;
            using PartitionClassPartition = PartitionT<PartitionClassArray>;
            using PartitionClassArrays = std::vector<PartitionClassArray>;

            using SolvableFraction = double;
            using SolvableFractionArray = PartitionedArray<SolvableFraction, 2>;
            using SolvableFractionPartitions = PartitionsT<SolvableFractionArray>;
            using SolvableFractionPartition = PartitionT<SolvableFractionArray>;
            using SolvableFractionArrays = std::vector<SolvableFractionArray>;

            using CountArray = PartitionedArray<std::uint32_t, 2>;
            using CountPartitions = PartitionsT<CountArray>;
            using CountPartition = PartitionT<CountArray>;
            using CountArrays = std::vector<CountArray>;

            using Shape = ShapeT<CellClassPartitions>;

            PartitionClassArrays partition_class_arrays{};
            SolvableFractionArrays solvable_fraction_arrays{};
            CountArrays nr_cells_to_solve_arrays{};

            Shape const& shape_in_partitions{cell_class_partitions.shape()};
            [[maybe_unused]] Count const nr_partitions{nr_elements(shape_in_partitions)};
            auto const [extent0, extent1] = shape_in_partitions;

            Count nr_iterations{0};
            std::vector<hpx::future<void>> accu_futures;

            using Action = AccuInfoInterPartitionStreamCellsAction<
                Policies,
                FlowDirectionPartition,
                PartitionIOComponent,
                InflowCountPartition,
                CellClassPartition,
                Accumulator>;

            Array<bool, 2> ready_partitions{shape_in_partitions, false};
            [[maybe_unused]] Count nr_ready_partitions{0};
            Action action;

            [[maybe_unused]] double hot_fraction = 1.0;

            // collection of (solvable_fraction, hot partition stuff) tuples
            std::vector<double> hot_partitions;

            while (true)
            {
                PartitionClassPartitions partition_class_partitions{shape_in_partitions};
                SolvableFractionPartitions solvable_fraction_partitions{shape_in_partitions};
                CountPartitions nr_cells_to_solve_partitions{shape_in_partitions};
                [[maybe_unused]] bool hot_partitions_seen{false};

                // Determine which partitions are part of the accumulation front
                auto [nr_input_cells, upstream_partition_offset_counts] =
                    flow_accumulation_front2(partition_io_partitions, ready_partitions);
                // lue_hpx_assert(nr_input_cells.shape() == material_partitions.shape());
                lue_hpx_assert(nr_input_cells.shape() == upstream_partition_offset_counts.shape());

                accu_futures.clear();
                Count nr_hot_partitions{0};
                Count nr_not_ready_partitions{0};

                // collection of (solvable_fraction, hot partition stuff) tuples
                hot_partitions.clear();

                for (Index idx0 = 0; idx0 < extent0; ++idx0)
                {
                    for (Index idx1 = 0; idx1 < extent1; ++idx1)
                    {
                        bool const partition_promise_is_ready{ready_partitions(idx0, idx1)};

                        PartitionClass partition_class = ready;
                        SolvableFraction solvable_fraction = 0.0;
                        Count nr_cells_to_solve{0};

                        if (!partition_promise_is_ready)
                        {
                            Count nr_solvable_cells{0};
                            nr_cells_to_solve = nr_input_cells(idx0, idx1);
                            auto const& offset_counts{upstream_partition_offset_counts(idx0, idx1)};

                            if (nr_cells_to_solve == 0)
                            {
                                // It assumed that this partition has
                                // been drained already, by the downstream
                                // partition.

                                // TODO Careful. This may not be the
                                //   case when a subset of the hot partitions
                                //   is used for calculations(?).

                                lue_hpx_assert(offset_counts.empty());

                                // Assign to the resulting output partition. This
                                // will mark the partition as ready, possibly
                                // allowing any continuations to be scheduled.
                                cell_class_promises(idx0, idx1)
                                    .set_value(cell_class_partitions(idx0, idx1).get_id());
                                ready_partitions(idx0, idx1) = true;
                            }
                            else if (!offset_counts.empty())
                            {
                                // offset_counts contains the offsets of direct
                                // neighbouring partitions from which we can
                                // receive material. Select the corresponding
                                // I/O components.

                                Array<PartitionIOComponent, 2> io_components{{3, 3}};

                                // Don't forget the center instance
                                // itself. It will be updated during
                                // the accumulation.
                                io_components(1, 1) = partition_io_partitions(idx0, idx1);

                                for (auto const& [offset, count] : offset_counts)
                                {
                                    io_components(1 + offset[0], 1 + offset[1]) =
                                        partition_io_partitions(idx0 + offset[0], idx1 + offset[1]);
                                    nr_solvable_cells += count;
                                }


                                typename PartitionIOComponent::PartitionOffsets offsets;
                                for (auto const& [offset, _] : offset_counts)
                                {
                                    offsets.insert(offset);
                                }

                                solvable_fraction =
                                    static_cast<SolvableFraction>(nr_solvable_cells) / nr_cells_to_solve;

                                hot_partitions.push_back(solvable_fraction);

                                // Spawn action that will forward material
                                // from surrounding partitions. This action
                                // updates the current inflow count and flux.
                                accu_futures.push_back(hpx::async(
                                    action,
                                    localities(idx0, idx1),
                                    policies,
                                    flow_direction_partitions(idx0, idx1),
                                    inflow_count_partitions(idx0, idx1),
                                    io_components,
                                    offsets,
                                    // material_partitions(idx0, idx1),
                                    cell_class_partitions(idx0, idx1)));

                                partition_class = hot;
                                hot_partitions_seen = true;

                                ++nr_hot_partitions;
                                ++nr_not_ready_partitions;
                            }
                            else
                            {
                                partition_class = cold;
                                ++nr_not_ready_partitions;
                            }
                        }

                        partition_class_partitions(idx0, idx1) = PartitionClassPartition{
                            localities(idx0, idx1), {idx0, idx1}, {1, 1}, partition_class};

                        solvable_fraction_partitions(idx0, idx1) = SolvableFractionPartition{
                            localities(idx0, idx1), {idx0, idx1}, {1, 1}, solvable_fraction};

                        nr_cells_to_solve_partitions(idx0, idx1) = CountPartition{
                            localities(idx0, idx1),
                            {idx0, idx1},
                            {1, 1},
                            static_cast<std::uint32_t>(nr_cells_to_solve)};
                    }
                }

                // No, in the last iterations these conditions don't hold.
                // lue_hpx_assert(!hot_partitions.empty());
                // lue_hpx_assert(hot_partitions_seen);

                // TODO Spawn tasks for hot partitions. Be smart about
                // which hot partitions to spawn tasks for.
                // ...

                [[maybe_unused]] Count nr_ready_partitions_new =
                    std::count(ready_partitions.begin(), ready_partitions.end(), true);
                lue_hpx_assert(nr_ready_partitions_new >= nr_ready_partitions || nr_iterations == 0);

                // Wait for all spawned actions to finish, if any
                hpx::wait_all(accu_futures);

                // The maximum number of iterations needed to
                // calculate a global solution must be equal to the
                // longest inter-partition stream, measured in number
                // of partitions it flows through. It is unlikely
                // that such a stream flows to three times the number of
                // all partitions(?). Let's abort in Debug mode if
                // that happens. Something is likely wrong then.
                ++nr_iterations;
                lue_hpx_assert(nr_iterations <= 3 * nr_partitions);

                lue_hpx_assert(all_are_valid(partition_class_partitions));
                partition_class_arrays.push_back(PartitionClassArray{
                    shape_in_partitions, localities, std::move(partition_class_partitions)});

                lue_hpx_assert(all_are_valid(solvable_fraction_partitions));
                solvable_fraction_arrays.push_back(SolvableFractionArray{
                    shape_in_partitions, localities, std::move(solvable_fraction_partitions)});

                lue_hpx_assert(all_are_valid(nr_cells_to_solve_partitions));
                nr_cells_to_solve_arrays.push_back(
                    CountArray{shape_in_partitions, localities, std::move(nr_cells_to_solve_partitions)});


                // Stop when no actions where spawned. This implies
                // that the global result is ready.
                if (accu_futures.empty())
                {
                    // // ----------------------------------
                    // // Useful for testing only.
                    // for(Index p = 0; p < nr_partitions; ++p)
                    // {
                    //     if(!ready_partitions[p])
                    //     {
                    //         cell_class_promises[p].set_value(cell_class_partitions[p].get_id());
                    //         ready_partitions[p] = true;
                    //     }
                    // }
                    // // ----------------------------------

                    lue_hpx_assert(std::all_of(
                        ready_partitions.begin(),
                        ready_partitions.end(),
                        [](bool const set) { return set; }));
                    break;
                }

                hot_fraction = static_cast<double>(nr_hot_partitions) / nr_not_ready_partitions;
            }

            lue_hpx_assert(static_cast<Count>(partition_class_arrays.size()) == nr_iterations);
            lue_hpx_assert(static_cast<Count>(solvable_fraction_arrays.size()) == nr_iterations);
            lue_hpx_assert(static_cast<Count>(nr_cells_to_solve_arrays.size()) == nr_iterations);

            partition_class_arrays_promise.set_value(std::move(partition_class_arrays));
            solvable_fraction_arrays_promise.set_value(std::move(solvable_fraction_arrays));
            nr_cells_to_solve_arrays_promise.set_value(std::move(nr_cells_to_solve_arrays));
        }


        template<
            typename Policies,
            typename CellClassPartition,
            typename PartitionIOComponent,
            typename InflowCountPartition,
            typename FlowDirectionPartitions,
            typename Accumulator>
        struct IntraPartitionStreamCellsAction:
            hpx::actions::make_action<
                decltype(&solve_intra_partition_stream_cells<
                         Policies,
                         CellClassPartition,
                         PartitionIOComponent,
                         InflowCountPartition,
                         FlowDirectionPartitions,
                         Accumulator>),
                &solve_intra_partition_stream_cells<
                    Policies,
                    CellClassPartition,
                    PartitionIOComponent,
                    InflowCountPartition,
                    FlowDirectionPartitions,
                    Accumulator>,
                IntraPartitionStreamCellsAction<
                    Policies,
                    CellClassPartition,
                    PartitionIOComponent,
                    InflowCountPartition,
                    FlowDirectionPartitions,
                    Accumulator>>::type
        {
        };


        template<
            typename Accumulator,
            typename Policies,
            typename Localities,
            typename FlowDirectionPartitions,
            typename PartitionIOComponents,
            typename InflowCountPartitions,
            typename CellClassPartitions>
        std::tuple<
            CellClassPartitions,
            hpx::future<std::vector<PartitionedArray<PartitionClass, 2>>>,
            hpx::future<std::vector<PartitionedArray<double, 2>>>,
            hpx::future<std::vector<PartitionedArray<std::uint32_t, 2>>>>
        solve_flow_accumulation(
            Policies const& policies,
            Localities const& localities,
            FlowDirectionPartitions const& flow_direction_partitions,
            PartitionIOComponents&& partition_io_partitions,
            InflowCountPartitions&& inflow_count_partitions,
            CellClassPartitions&& current_cell_class_partitions)
        {
            // This function must return results as quickly as possible

            // Create a collection of array partition clients. Each of
            // these partitions will be connected to an array partition
            // server once these are calculated. Some partitions will
            // be connected sooner than others, depending on the flow
            // direction network.
            // Meanwhile, other modelling operations can attach
            // continuations to the partitions.

            using CellClassPartition = PartitionT<CellClassPartitions>;
            using Shape = ShapeT<FlowDirectionPartitions>;

            Shape const& shape_in_partitions{current_cell_class_partitions.shape()};

            // For each partition a promise of a partition server instance ID
            IDPromiseArray<2> cell_class_partitions_component_ids{shape_in_partitions};

            // For each partition a client instance, that contains a
            // future to a partition server instance ID
            CellClassPartitions result_cell_class_partitions{shape_in_partitions};

            Count const nr_partitions{nr_elements(shape_in_partitions)};

            for (Index p = 0; p < nr_partitions; ++p)
            {
                // Connect the array with promises with the array of futures
                result_cell_class_partitions[p] =
                    CellClassPartition{cell_class_partitions_component_ids[p].get_future()};
                lue_hpx_assert(result_cell_class_partitions[p].valid());
            }

            // A promise of a vector of partition class arrays
            using PartitionClassArray = PartitionedArray<PartitionClass, 2>;
            using PartitionClassArrays = std::vector<PartitionClassArray>;
            using PartitionClassArraysPromise = hpx::promise<PartitionClassArrays>;
            using PartitionClassArraysFuture = hpx::future<PartitionClassArrays>;

            PartitionClassArraysPromise partition_class_arrays_promise;
            PartitionClassArraysFuture partition_class_arrays_future{
                partition_class_arrays_promise.get_future()};

            using SolvableFraction = double;
            using SolvableFractionArray = PartitionedArray<SolvableFraction, 2>;
            using SolvableFractionArrays = std::vector<SolvableFractionArray>;
            using SolvableFractionArraysPromise = hpx::promise<SolvableFractionArrays>;
            using SolvableFractionArraysFuture = hpx::future<SolvableFractionArrays>;

            SolvableFractionArraysPromise solvable_fraction_arrays_promise;
            SolvableFractionArraysFuture solvable_fraction_arrays_future{
                solvable_fraction_arrays_promise.get_future()};

            using CountArray = PartitionedArray<std::uint32_t, 2>;
            using CountArrays = std::vector<CountArray>;
            using CountArraysPromise = hpx::promise<CountArrays>;
            using CountArraysFuture = hpx::future<CountArrays>;

            CountArraysPromise nr_cells_to_solve_arrays_promise;
            CountArraysFuture nr_cells_to_solve_arrays_future{nr_cells_to_solve_arrays_promise.get_future()};

            // Asynchronously start the calculations that will
            // result in the global flow accumulation result.
            // During the calculations, the promises of the individual
            // partition server instance IDs will be fulfilled, as soon
            // as possible. Fire and forget.
            hpx::post(
                solve_inter_partition_stream_cells<
                    Policies,
                    Localities,
                    FlowDirectionPartitions,
                    PartitionIOComponents,
                    InflowCountPartitions,
                    CellClassPartitions,
                    Accumulator>,
                policies,
                localities,
                flow_direction_partitions,
                std::move(partition_io_partitions),
                std::move(inflow_count_partitions),
                std::move(current_cell_class_partitions),
                std::move(cell_class_partitions_component_ids),
                std::move(partition_class_arrays_promise),
                std::move(solvable_fraction_arrays_promise),
                std::move(nr_cells_to_solve_arrays_promise));

            return std::make_tuple(
                std::move(result_cell_class_partitions),
                std::move(partition_class_arrays_future),
                std::move(solvable_fraction_arrays_future),
                std::move(nr_cells_to_solve_arrays_future));
        }


        template<typename Policies, typename FlowDirectionElement, typename Accumulator, Rank rank>
        std::tuple<
            PartitionedArray<CellClass, rank>,
            hpx::future<std::vector<PartitionedArray<PartitionClass, rank>>>,
            hpx::future<std::vector<PartitionedArray<double, rank>>>,
            hpx::future<std::vector<PartitionedArray<std::uint32_t, rank>>>>
        flow_accumulation(
            Policies const& policies,
            PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
            [[maybe_unused]] Accumulator&& accumulator)
        {
            lue::Localities<rank> const& localities{flow_direction.localities()};

            using FlowDirectionArray = PartitionedArray<FlowDirectionElement, rank>;


            // ---------------------------------------------------------------------
            // Create halo partitions around input arrays
            ShapeT<FlowDirectionArray> min_shape{};
            std::fill(min_shape.begin(), min_shape.end(), 1);

            auto const halo_partitions = detail::halo_partitions(
                std::get<0>(policies.inputs_policies()),
                localities,
                min_shape,
                detail::partitions(flow_direction));


            // ---------------------------------------------------------------------
            // Calculate flow accumulation for intra-partition stream cells
            using InflowCountElement = std::uint8_t;
            using InflowCountArray = PartitionedArray<InflowCountElement, rank>;
            using CellClassArray = PartitionedArray<CellClass, rank>;

            using PartitionIOArray =
                ComponentArray<lue::ArrayPartitionIO<lue::Index, rank, typename Accumulator::Material>, rank>;

            using Action = IntraPartitionStreamCellsAction<
                Policies,
                PartitionT<CellClassArray>,
                ElementT<PartitionIOArray>,
                PartitionT<InflowCountArray>,
                PartitionsT<FlowDirectionArray>,
                Accumulator>;

            auto [cell_class_partitions, partition_io_partitions, inflow_count_partitions] =
                detail::spawn_components<
                    PartitionsT<CellClassArray>,
                    ElementsT<PartitionIOArray>,
                    PartitionsT<InflowCountArray>>(
                    policies, Action{}, localities, halo_partitions, detail::partitions(flow_direction));


            // ---------------------------------------------------------------------
            // Calculate flow accumulation for inter-partition stream cells
            auto
                [cell_class_partitions2,
                 partition_class_arrays,
                 solvable_fraction_arrays,
                 nr_cells_to_solve_arrays] =
                    solve_flow_accumulation<Accumulator>(
                        policies,
                        localities,
                        detail::partitions(flow_direction),
                        std::move(partition_io_partitions),
                        std::move(inflow_count_partitions),
                        std::move(cell_class_partitions));

            return std::make_tuple(
                std::move(
                    CellClassArray{shape(flow_direction), localities, std::move(cell_class_partitions2)}),
                std::move(partition_class_arrays),
                std::move(solvable_fraction_arrays),
                std::move(nr_cells_to_solve_arrays));
        }

    }  // namespace detail::accu_info


    /*!
        @brief      Determine information about the flow accumulation
                    algorithm
        @param      .
        @return     Tuple with collected information
        @exception  .

        - Array with per cell whether it is
          - part of an intra-partition stream
          - an inter-partition stream, and if so, whether it is also
              - a partition input cell
              - a partition output cell
        - Vector of arrays, with per partition whether it is ready,
          hot or cold
    */
    template<typename Policies, typename FlowDirectionElement, Rank rank>
    std::tuple<
        PartitionedArray<CellClass, rank>,
        hpx::future<std::vector<PartitionedArray<PartitionClass, rank>>>,
        hpx::future<std::vector<PartitionedArray<double, rank>>>,
        hpx::future<std::vector<PartitionedArray<std::uint32_t, rank>>>>
    accu_info(Policies const& policies, PartitionedArray<FlowDirectionElement, rank> const& flow_direction)
    {
        // NOTE: Only make this function return information that
        // cannot be obtained easily otherwise.
        // - Stream heads: use inflow_count == 0
        // - Stream order: if needed, add operations for it:
        //   strahler_stream_order, shreve_stream_order,
        //   horton_stream_order, ...

        using Accumulator = detail::accu_info::Accumulator<Index, rank>;

        return detail::accu_info::flow_accumulation(policies, flow_direction, Accumulator{});
    }

}  // namespace lue


#define LUE_INSTANTIATE_ACCU_INFO(Policies, FlowDirectionElement)                                            \
                                                                                                             \
    template LUE_ROUTING_OPERATION_EXPORT std::tuple<                                                        \
        PartitionedArray<CellClass, 2>,                                                                      \
        hpx::future<std::vector<PartitionedArray<PartitionClass, 2>>>,                                       \
        hpx::future<std::vector<PartitionedArray<double, 2>>>,                                               \
        hpx::future<std::vector<PartitionedArray<std::uint32_t, 2>>>>                                        \
    accu_info<ArgumentType<void(Policies)>, FlowDirectionElement, 2>(                                        \
        ArgumentType<void(Policies)> const&, PartitionedArray<FlowDirectionElement, 2> const&);
