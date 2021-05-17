#pragma once
#include "lue/framework/algorithm/detail/accumulate.hpp"
#include "lue/framework/algorithm/detail/halo_partition.hpp"
#include "lue/framework/algorithm/detail/promise.hpp"
#include "lue/framework/algorithm/component/array_partition_io.hpp"
#include "lue/framework/algorithm/inflow_count.hpp"
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/algorithm/type_traits.hpp"
#include "lue/framework/partitioned_array.hpp"
#include "lue/framework/core/assert.hpp"
#include "lue/framework/core/component/component_array.hpp"
#include "lue/framework/core/serialize/array.hpp"


namespace lue {
    namespace detail {

        template<
            typename MaterialElement>
        class AccuThresholdMaterial
        {

            public:

                using FluxElement = MaterialElement;
                using StateElement = MaterialElement;

                AccuThresholdMaterial()=default;


                AccuThresholdMaterial(
                    std::tuple<FluxElement, StateElement> material):

                    _flux{std::get<0>(material)},
                    _state{std::get<1>(material)}

                {
                }


                std::tuple<FluxElement const&, StateElement const&> operator()() const
                {
                    return std::forward_as_tuple(_flux, _state);
                }


            private:

                friend class hpx::serialization::access;


                void serialize(
                    hpx::serialization::input_archive& archive,
                    [[maybe_unused]] unsigned int const version)
                {
                    archive & _flux & _state;
                }


                void serialize(
                    hpx::serialization::output_archive& archive,
                    [[maybe_unused]] unsigned int const version) const
                {
                    archive & _flux & _state;
                }


                FluxElement _flux;

                StateElement _state;

        };


        namespace accu_threshold {

            // This class is needed for keeping track of information
            // that is needed to insert external material into a cell.
            template<
                typename MaterialCells>
            class InputMaterial
            {

                public:

                    using InputMaterialElement = ElementT<MaterialCells>;
                    using ThresholdElement = ElementT<MaterialCells>;
                    using InputElement = std::tuple<InputMaterialElement const&, ThresholdElement const&>;


                    InputMaterial(
                        MaterialCells const& material,
                        MaterialCells const& threshold):

                        _material{material},
                        _threshold{threshold}

                    {
                    }


                    InputElement operator()(
                        Index const idx0,
                        Index const idx1) const
                    {
                        return std::forward_as_tuple(_material(idx0, idx1), _threshold(idx0, idx1));
                    }


                private:

                    MaterialCells const& _material;

                    MaterialCells const& _threshold;

            };


            // This class template is for keeping track of the information
            // required to calculate accumulated material to
            // downstream cells.
            template<
                typename MaterialCells>
            class OutputMaterial
            {

                public:

                    using FluxElement = ElementT<MaterialCells>;
                    using StateElement = ElementT<MaterialCells>;
                    using OutputElement = std::tuple<FluxElement&, StateElement&>;

                    using Element = std::tuple<FluxElement, StateElement>;


                    OutputMaterial(
                        MaterialCells& flux,
                        MaterialCells& state):

                        _flux{flux},
                        _state{state}

                    {
                    }


                    OutputElement operator()(
                        Index const idx0,
                        Index const idx1)
                    {
                        return std::forward_as_tuple(_flux(idx0, idx1), _state(idx0, idx1));
                    }


                private:

                    MaterialCells& _flux;

                    MaterialCells& _state;

            };


            template<
                typename InputMaterialNoDataPolicy,
                typename InputThresholdNoDataPolicy,
                typename OutputFluxNoDataPolicy,
                typename OutputStateNoDataPolicy>
            class CellAccumulator
            {

                public:

                    using InputMaterialElement = policy::ElementT<InputMaterialNoDataPolicy>;
                    using InputThresholdElement = policy::ElementT<InputThresholdNoDataPolicy>;
                    using OutputFluxElement = policy::ElementT<OutputFluxNoDataPolicy>;
                    using OutputStateElement = policy::ElementT<OutputStateNoDataPolicy>;

                    using InputElement = std::tuple<InputMaterialElement const&, InputThresholdElement const&>;
                    using OutputElement = std::tuple<OutputFluxElement&, OutputStateElement&>;
                    using ConstOutputElement = std::tuple<OutputFluxElement const&, OutputStateElement const&>;


                    CellAccumulator(
                        InputMaterialNoDataPolicy const& indp_material,
                        InputThresholdNoDataPolicy const& indp_threshold,
                        OutputFluxNoDataPolicy const& ondp_flux,
                        OutputStateNoDataPolicy const& ondp_state):

                        _indp_material{indp_material},
                        _indp_threshold{indp_threshold},
                        _ondp_flux{ondp_flux},
                        _ondp_state{ondp_state}

                    {
                    }


                    void accumulate_input(
                        InputElement const& input_element,
                        OutputElement output_element) const
                    {
                        auto const& [material, threshold] = input_element;
                        auto& [flux, state] = output_element;

                        lue_hpx_assert(
                            (_ondp_flux.is_no_data(flux) && _ondp_state.is_no_data(state)) ||
                            (!_ondp_flux.is_no_data(flux) && !_ondp_state.is_no_data(state)));

                        if(!_ondp_flux.is_no_data(flux))
                        {
                            if(_indp_material.is_no_data(material) ||
                                _indp_threshold.is_no_data(threshold))
                            {
                                _ondp_flux.mark_no_data(flux);
                                _ondp_state.mark_no_data(state);
                            }
                            else
                            {
                                // TODO Domain check
                                lue_hpx_assert(threshold >= 0);

                                // Now we know the final, total amount
                                // of material that enters this cell
                                flux += material;

                                // Split this amount into flux and state,
                                // based on the threshold passed in

                                OutputFluxElement mobile_material{flux - threshold};

                                if(mobile_material > 0)
                                {
                                    state = threshold;
                                    flux = mobile_material;
                                }
                                else
                                {
                                    state = flux;
                                    flux = 0;
                                }
                            }
                        }
                    }


                    void accumulate_downstream(
                        ConstOutputElement const& upstream_material_element,
                        OutputElement downstream_material_element) const
                    {
                        auto const& [upstream_flux, upstream_state] = upstream_material_element;
                        auto& [downstream_flux, downstream_state] = downstream_material_element;

                        if(!_ondp_flux.is_no_data(downstream_flux))
                        {
                            lue_hpx_assert(!_ondp_state.is_no_data(downstream_state));

                            if(_ondp_flux.is_no_data(upstream_flux))
                            {
                                lue_hpx_assert(_ondp_state.is_no_data(upstream_state));

                                _ondp_flux.mark_no_data(downstream_flux);
                                _ondp_state.mark_no_data(downstream_state);
                            }
                            else
                            {
                                // Just push the upstream flux to the downstream cell
                                downstream_flux += upstream_flux;
                            }
                        }
                    }


                private:

                    InputMaterialNoDataPolicy _indp_material;

                    InputThresholdNoDataPolicy _indp_threshold;

                    OutputFluxNoDataPolicy _ondp_flux;

                    OutputStateNoDataPolicy _ondp_state;

            };

        }  // namespace accu_threshold


        template<
            typename Policies,
            typename PartitionIOComponent,
            typename InflowCountPartition,
            typename FlowDirectionPartitions,
            typename MaterialPartitions>
        hpx::util::tuple<
            PartitionIOComponent,
            InflowCountPartition,
            PartitionT<MaterialPartitions>,
            PartitionT<MaterialPartitions>>
                accumulate_threshold_intra_partition_stream_cells(
                    Policies const& policies,
                    OffsetT<PartitionT<FlowDirectionPartitions>> const& partition_offset,
                    FlowDirectionPartitions const& flow_direction_partitions,
                    MaterialPartitions const& material_partitions,
                    MaterialPartitions const& threshold_partitions)
        {
            using FlowDirectionPartition = PartitionT<FlowDirectionPartitions>;
            using FlowDirectionData = DataT<FlowDirectionPartition>;
            using FlowDirectionElement = ElementT<FlowDirectionPartitions>;

            using MaterialPartition = PartitionT<MaterialPartitions>;
            using MaterialData = DataT<MaterialPartition>;

            using PartitionIOData = typename PartitionIOComponent::Data;
            using InflowCountData = DataT<InflowCountPartition>;
            using InflowCountElement = ElementT<InflowCountPartition>;

            using Offset = OffsetT<FlowDirectionPartition>;

            hpx::future<Offset> offset{flow_direction_partitions(1, 1).offset()};

            // Determine shapes of all partitions
            auto const partition_shapes = detail::partition_shapes(flow_direction_partitions);

            // Request the minimum amount of required data from all partitions
            auto flow_direction_data = partition_data(flow_direction_partitions, partition_shapes);
            auto input_material_data = partition_data(material_partitions, partition_shapes);
            auto threshold_data = partition_data(threshold_partitions, partition_shapes);

            // Once all input data has arrived...
            return hpx::split_future(hpx::dataflow(
                hpx::launch::async,
                hpx::util::unwrapping(

                        [policies, partition_offset](
                            Offset offset,
                            lue::Array<FlowDirectionData, rank<FlowDirectionData>> const& flow_direction_data,
                            lue::Array<MaterialData, rank<MaterialData>> const& input_material_data,
                            lue::Array<MaterialData, rank<MaterialData>> const& threshold_data)
                        {
                            // -------------------------------------------------
                            // Determine inflow_count
                            // As long as we only use flow_direction
                            // and material to detect no-data in input,
                            // there is no need to mark no-data in output
                            // of inflow_count
                            using InflowCountOutputPolicies =
                                policy::OutputPolicies<policy::DontMarkNoData<InflowCountElement>>;
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

                            auto [inflow_count_data, input_cell_idxs] =
                                detail::inflow_count_data<InflowCountData>(
                                    inflow_count_policies, flow_direction_data);
                            lue_hpx_assert(std::is_sorted(input_cell_idxs.begin(), input_cell_idxs.end()));


                            // -------------------------------------------------
                            // Determine PartitionIO and calculate flow
                            // accumulation for intra-partition cells
                            auto const& indp_flow_direction =
                                std::get<0>(policies.inputs_policies()).input_no_data_policy();
                            auto const& indp_material =
                                std::get<1>(policies.inputs_policies()).input_no_data_policy();
                            auto const& indp_threshold =
                                std::get<2>(policies.inputs_policies()).input_no_data_policy();
                            auto const& ondp_flux =
                                std::get<0>(policies.outputs_policies()).output_no_data_policy();
                            auto const& ondp_state =
                                std::get<1>(policies.outputs_policies()).output_no_data_policy();

                            using CellAccumulator = accu_threshold::CellAccumulator<
                                decltype(indp_material), decltype(indp_threshold),
                                decltype(ondp_flux), decltype(ondp_state)>;

                            using Shape = ShapeT<InflowCountData>;
                            Shape const partition_shape{inflow_count_data.shape()};

                            PartitionIOData partition_io_data{partition_shape, std::move(input_cell_idxs)};
                            MaterialData flux_data{partition_shape, 0};
                            MaterialData state_data{partition_shape, 0};

                            using InputMaterial = accu_threshold::InputMaterial<MaterialData>;
                            using OutputMaterial = accu_threshold::OutputMaterial<MaterialData>;
                            using Accumulator = detail::Accumulator<
                                PartitionIOData, InputMaterial, CellAccumulator, OutputMaterial>;

                            InputMaterial input_material{input_material_data(1, 1), threshold_data(1, 1)};
                            OutputMaterial output_material{flux_data, state_data};

                            Accumulator accumulator{
                                partition_io_data,
                                input_material,
                                CellAccumulator{indp_material, indp_threshold, ondp_flux, ondp_state},
                                output_material};

                            auto const [nr_elements0, nr_elements1] = partition_shape;

                            InflowCountData inflow_count_data_copy{deep_copy(inflow_count_data)};

                            for(Index idx0 = 0; idx0 < nr_elements0; ++idx0) {
                                for(Index idx1 = 0; idx1 < nr_elements1; ++idx1)
                                {
                                    if(indp_flow_direction.is_no_data(flow_direction_data(1, 1), idx0, idx1))
                                    {
                                        // Skip cells for which we don't have a flow-direction
                                        ondp_flux.mark_no_data(flux_data, idx0, idx1);
                                        ondp_state.mark_no_data(state_data, idx0, idx1);
                                    }
                                    else if(inflow_count_data(idx0, idx1) == 0)
                                    {
                                        detail::accumulate(
                                            accumulator, idx0, idx1, flow_direction_data(1, 1),
                                            inflow_count_data_copy);
                                    }
                                }
                            }


                            // -------------------------------------------------
                            // Return all results
                            return hpx::util::make_tuple(
                                    PartitionIOComponent{
                                        hpx::find_here(), std::move(partition_io_data)},
                                    InflowCountPartition{
                                        hpx::find_here(), offset, std::move(inflow_count_data_copy)},
                                    MaterialPartition{
                                        hpx::find_here(), offset, std::move(flux_data)},
                                    MaterialPartition{
                                        hpx::find_here(), offset, std::move(state_data)}
                                );
                        }

                    ),
                std::move(offset),
                when_all_get(std::move(flow_direction_data)),
                when_all_get(std::move(input_material_data)),
                when_all_get(std::move(threshold_data))));
        }


        template<
            typename Policies,
            typename PartitionIOComponent,
            typename InflowCountPartition,
            typename FlowDirectionPartitions,
            typename MaterialPartitions>
        struct AccumulateThresholdIntraPartitionStreamCellsAction:
            hpx::actions::make_action<
                decltype(&accumulate_threshold_intra_partition_stream_cells<
                    Policies, PartitionIOComponent, InflowCountPartition,
                    FlowDirectionPartitions, MaterialPartitions>),
                &accumulate_threshold_intra_partition_stream_cells<
                    Policies, PartitionIOComponent, InflowCountPartition,
                    FlowDirectionPartitions, MaterialPartitions>,
                AccumulateThresholdIntraPartitionStreamCellsAction<
                    Policies, PartitionIOComponent, InflowCountPartition,
                    FlowDirectionPartitions, MaterialPartitions>>::type
        {};


        template<
            typename Policies,
            typename FlowDirectionPartition,
            typename PartitionIOComponent,
            typename InflowCountPartition,
            typename MaterialPartition>
        void accumulate_threshold_inter_partition_stream_cells(
            Policies const& policies,
            FlowDirectionPartition const& flow_direction_partition,
            InflowCountPartition const& inflow_count_partition,
            Array<PartitionIOComponent, 2> io_components,
            typename PartitionIOComponent::PartitionOffsets const& offsets,
            MaterialPartition const& material_partition,
            MaterialPartition const& threshold_partition,
            MaterialPartition const& flux_partition,
            MaterialPartition const& state_partition)
        {
lue_hpx_assert(material_partition.is_ready());
            using MaterialData = DataT<MaterialPartition>;
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

            auto material_partition_ptr{detail::ready_component_ptr(material_partition)};
            auto& material_partition_server{*material_partition_ptr};
            auto material_data{material_partition_server.data()};

            auto threshold_partition_ptr{detail::ready_component_ptr(threshold_partition)};
            auto& threshold_partition_server{*threshold_partition_ptr};
            auto threshold_data{threshold_partition_server.data()};

            auto flux_partition_ptr{detail::ready_component_ptr(flux_partition)};
            auto& flux_partition_server{*flux_partition_ptr};
            auto flux_data{flux_partition_server.data()};

            auto state_partition_ptr{detail::ready_component_ptr(state_partition)};
            auto& state_partition_server{*state_partition_ptr};
            auto state_data{state_partition_server.data()};

            Shape const& partition_shape{flow_direction_partition_server.shape()};

            using Indices = typename PartitionIOComponent::Indices;
            using MaterialElement = typename PartitionIOComponent::Value;

            std::vector<hpx::future<std::vector<std::tuple<Indices, MaterialElement>>>> input_cells_f;
            input_cells_f.reserve(offsets.size());

            // Iterate over the neighbouring partitions that have material
            // for us.
            for(Offset const& offset: offsets)
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
                input_cells_f.begin(), input_cells_f.end(), input_cells_.begin(),
                [](auto& future)
                {
                    return future.get();
                });

            // Neighbours have drained. Now we need to push the material
            // we received through our partition.

            auto partition_io_ptr{detail::ready_component_ptr(io_components(1, 1))};
            auto& partition_io_server{*partition_io_ptr};
            auto& partition_io_data{partition_io_server.data()};

            auto const& indp_material =
                std::get<1>(policies.inputs_policies()).input_no_data_policy();
            auto const& indp_threshold =
                std::get<2>(policies.inputs_policies()).input_no_data_policy();
            auto const& ondp_flux =
                std::get<0>(policies.outputs_policies()).output_no_data_policy();
            auto const& ondp_state =
                std::get<1>(policies.outputs_policies()).output_no_data_policy();

            using PartitionIOData = typename PartitionIOComponent::Data;

            using CellAccumulator = accu_threshold::CellAccumulator<
                decltype(indp_material), decltype(indp_threshold),
                decltype(ondp_flux), decltype(ondp_state)>;
            using InputMaterial = accu_threshold::InputMaterial<MaterialData>;
            using OutputMaterial = accu_threshold::OutputMaterial<MaterialData>;
            using Accumulator = detail::Accumulator<
                PartitionIOData, InputMaterial, CellAccumulator, OutputMaterial>;
            using Accumulator =
                detail::Accumulator<PartitionIOData, InputMaterial, CellAccumulator, OutputMaterial>;

            InputMaterial input_material{material_data, threshold_data};
            OutputMaterial output_material{flux_data, state_data};

            Accumulator accumulator{
                partition_io_data,
                input_material,
                CellAccumulator{indp_material, indp_threshold, ondp_flux, ondp_state},
                output_material};

            for(auto const& input_cells: input_cells_)
            {
                lue_hpx_assert(!input_cells.empty());

                for(auto const& [input_cell_idxs, material]: input_cells)
                {
                    // Insert material at input cell and
                    // propagate as far as we can

                    // In case the material is no-data, propagate this
                    // in downstream direction

                    auto const [idx0, idx1] = input_cell_idxs;

                    lue_hpx_assert(inflow_count_data(idx0, idx1) >= 1);

                    accumulator.cell_accumulator().accumulate_downstream(
                        material(), output_material(idx0, idx1));

                    --inflow_count_data(idx0, idx1);
                    partition_io_data.remove_input_cell(input_cell_idxs);

                    if(inflow_count_data(idx0, idx1) == 0)
                    {
                        // Push material downstream until a cell is
                        // reached with inflow count > 1.
                        detail::accumulate(accumulator, idx0, idx1, flow_direction_data, inflow_count_data);
                    }
                }
            }
        }


        template<
            typename Policies,
            typename FlowDirectionPartition,
            typename PartitionIOComponent,
            typename InflowCountPartition,
            typename MaterialPartition>
        struct AccumulateThresholdInterPartitionStreamCellsAction:
            hpx::actions::make_action<
                decltype(&accumulate_threshold_inter_partition_stream_cells<
                    Policies, FlowDirectionPartition, PartitionIOComponent, InflowCountPartition,
                    MaterialPartition>),
                &accumulate_threshold_inter_partition_stream_cells<
                    Policies, FlowDirectionPartition, PartitionIOComponent, InflowCountPartition,
                    MaterialPartition>,
                AccumulateThresholdInterPartitionStreamCellsAction<
                    Policies, FlowDirectionPartition, PartitionIOComponent, InflowCountPartition,
                    MaterialPartition>>::type
        {};


        template<
            typename Policies,
            typename Localities,
            typename FlowDirectionPartitions,
            typename PartitionIOComponents,
            typename InflowCountPartitions,
            typename MaterialPartitions>
        void solve_accu_threshold_array(
            Policies const& policies,
            Localities const& localities,
            FlowDirectionPartitions const& flow_direction_partitions,
            PartitionIOComponents&& partition_io_partitions,
            InflowCountPartitions&& inflow_count_partitions,
            MaterialPartitions const& material_partitions,
            MaterialPartitions const& threshold_partitions,
            MaterialPartitions&& flux_partitions,
            MaterialPartitions&& state_partitions,
            IDPromiseArray<rank<MaterialPartitions>>&& flux_promises,
            IDPromiseArray<rank<MaterialPartitions>>&& state_promises)
        {
lue_hpx_assert(all_are_ready(material_partitions));
lue_hpx_assert(material_partitions(0, 0).is_ready());
            // Perform as many iterations as needed to calculate a global
            // flow accumulation solution

            // As soon as a partition is ready, set the corresponding
            // promise's ID. This will allow any continuations to be
            // scheduled for execution, even though not all partitions
            // calculated here are ready yet.

            using FlowDirectionPartition = ComponentT<FlowDirectionPartitions>;
            using PartitionIOComponent = ComponentT<PartitionIOComponents>;
            using InflowCountPartition = ComponentT<InflowCountPartitions>;
            using MaterialPartition = ComponentT<MaterialPartitions>;

            using Shape = ShapeT<MaterialPartitions>;

            Shape const& shape_in_partitions{flux_partitions.shape()};
            [[maybe_unused]] Count const nr_partitions{nr_elements(shape_in_partitions)};
            auto const [ extent0, extent1 ] = shape_in_partitions;

            Count nr_iterations{0};
            std::vector<hpx::future<void>> accu_futures;

            using Action = detail::AccumulateThresholdInterPartitionStreamCellsAction<
                Policies,
                FlowDirectionPartition, PartitionIOComponent, InflowCountPartition,
                MaterialPartition>;

            Array<bool, 2> ready_partitions{shape_in_partitions, false};
            [[maybe_unused]] Count nr_ready_partitions{0};
            Action action;

            while(true)
            {
lue_hpx_assert(material_partitions(0, 0).is_ready());
                // Determine which partitions are part of the accumulation front
                auto [ solved_partitions, upstream_partition_offsets ] =
                    flow_accumulation_front(partition_io_partitions, ready_partitions);
                lue_hpx_assert(solved_partitions.shape() == material_partitions.shape());
                lue_hpx_assert(upstream_partition_offsets.shape() == upstream_partition_offsets.shape());
lue_hpx_assert(material_partitions(0, 0).is_ready());

                accu_futures.clear();

                for(Index idx0 = 0; idx0 < extent0; ++idx0) {
                    for(Index idx1 = 0; idx1 < extent1; ++idx1)
                    {
lue_hpx_assert(material_partitions(idx0, idx1).is_ready());
                        bool const partition_promise_is_ready{ready_partitions(idx0, idx1)};

                        if(!partition_promise_is_ready)
                        {
                            bool const partition_is_solved{solved_partitions(idx0, idx1)};
                            auto const& offsets{upstream_partition_offsets(idx0, idx1)};

                            if(partition_is_solved)
                            {
                                // It assumed that this partition has
                                // been drained already, by the downstream
                                // partition.
                                lue_hpx_assert(offsets.empty());

                                // Assign to the resulting output partition. This
                                // will mark the partition as ready, possibly
                                // allowing any continuations to be scheduled.
                                flux_promises(idx0, idx1).set_value(flux_partitions(idx0, idx1).get_id());
                                state_promises(idx0, idx1).set_value(state_partitions(idx0, idx1).get_id());
                                ready_partitions(idx0, idx1) = true;
                            }
                            else if(!offsets.empty())
                            {
                                // offsets contains the offsets of direct
                                // neighbouring partitions from which we can
                                // receive material. Select the corresponding
                                // I/O components.

                                Array<PartitionIOComponent, 2> io_components{{3, 3}};

                                // Don't forget the center instance
                                // itself. It will be updated during
                                // the accumulation.
                                io_components(1, 1) = partition_io_partitions(idx0, idx1);

                                for(auto const& offset: offsets)
                                {
                                    io_components(1 + offset[0], 1 + offset[1]) =
                                        partition_io_partitions(idx0 + offset[0], idx1 + offset[1]);
                                }

                                // Spawn action that will forward material
                                // from surrounding partitions. This action
                                // updates the current inflow count and flux.

lue_hpx_assert(material_partitions(idx0, idx1).is_ready());
                                accu_futures.push_back(hpx::async(action, localities(idx0, idx1),
                                    policies,
                                    flow_direction_partitions(idx0, idx1), inflow_count_partitions(idx0, idx1),
                                    io_components, offsets,
                                    material_partitions(idx0, idx1),
                                    threshold_partitions(idx0, idx1),
                                    flux_partitions(idx0, idx1),
                                    state_partitions(idx0, idx1)));
                            }
                        }
                    }
                }

                [[maybe_unused]] Count nr_ready_partitions_new =
                    std::count(ready_partitions.begin(), ready_partitions.end(), true);
                lue_hpx_assert(nr_ready_partitions_new >= nr_ready_partitions || nr_iterations == 0);

                // Stop when there are no actions spawned. This implies
                // that the global result is ready.
                if(accu_futures.empty())
                {

                    // // ----------------------------------
                    // // Useful for testing only.
                    // for(Index p = 0; p < nr_partitions; ++p)
                    // {
                    //     if(!ready_partitions[p])
                    //     {
                    //         flux_promises[p].set_value(flux_partitions[p].get_id());
                    //         ready_partitions[p] = true;
                    //     }
                    // }
                    // // ----------------------------------

                    lue_hpx_assert(std::all_of(ready_partitions.begin(), ready_partitions.end(),
                        [](bool const set) { return set; }));
                    break;
                }

                // Wait for all spawned actions to finish
                hpx::wait_all(accu_futures);

                // The maximum number of iterations needed to
                // calculate a global solution must be equal to the
                // longest inter-partition stream, measured in number
                // of partitions it flows through. It is unlikely
                // that such a stream flows to three times the number of
                // all partitions. Let's abort in Debug mode if
                // that happens. Something is likely wrong then.
                ++nr_iterations;
                lue_hpx_assert(nr_iterations <= 3 * nr_partitions);
            }
        }


        template<
            typename Policies,
            typename Localities,
            typename FlowDirectionPartitions,
            typename PartitionIOComponents,
            typename InflowCountPartitions,
            typename MaterialPartitions>
        std::tuple<MaterialPartitions, MaterialPartitions> solve_accu_threshold(
            Policies const& policies,
            Localities const& localities,
            FlowDirectionPartitions const& flow_direction_partitions,
            PartitionIOComponents&& partition_io_partitions,
            InflowCountPartitions&& inflow_count_partitions,
            MaterialPartitions const& material_partitions,
            MaterialPartitions const& threshold_partitions,
            MaterialPartitions&& current_flux_partitions,
            MaterialPartitions&& current_state_partitions)
        {
            // This function must return results as quickly as possible

            // Create a collection of array partition clients. Each of
            // these partitions will be connected to an array partition
            // server once these are calculated. Some partitions will
            // be connected sooner than others, depending on the flow
            // direction network.
            // Meanwhile, other modelling operations can attach
            // continuations to the partitions.

            using MaterialPartition = PartitionT<MaterialPartitions>;
            using Shape = ShapeT<MaterialPartitions>;

            Shape const& shape_in_partitions{current_flux_partitions.shape()};

            // For each partition a promise of a partition server instance ID
            IDPromiseArray<2> flux_partitions_component_ids{shape_in_partitions};
            IDPromiseArray<2> state_partitions_component_ids{shape_in_partitions};

            // For each partition a client instance, that contains a
            // future to a partition server instance ID
            MaterialPartitions result_flux_partitions{shape_in_partitions};
            MaterialPartitions result_state_partitions{shape_in_partitions};

            Count const nr_partitions{nr_elements(shape_in_partitions)};

            for(Index p = 0; p < nr_partitions; ++p)
            {
                // Connect the array with promises with the array of futures
                result_flux_partitions[p] = MaterialPartition{flux_partitions_component_ids[p].get_future()};
                lue_hpx_assert(result_flux_partitions[p].valid());

                result_state_partitions[p] = MaterialPartition{state_partitions_component_ids[p].get_future()};
                lue_hpx_assert(result_state_partitions[p].valid());
            }

            // Asynchronously start the calculations that will
            // result in the global flow accumulation result.
            // During the calculations, the promises of the individual
            // partition server instance IDs will be fulfilled, as soon
            // as possible. Fire and forget.
lue_hpx_assert(all_are_ready(material_partitions));
            hpx::apply(
                solve_accu_threshold_array<Policies, Localities,
                    FlowDirectionPartitions, PartitionIOComponents, InflowCountPartitions,
                    MaterialPartitions>,
                policies,
                localities,
                flow_direction_partitions,
                std::move(partition_io_partitions),
                std::move(inflow_count_partitions),
                material_partitions, threshold_partitions,
                std::move(current_flux_partitions),
                std::move(current_state_partitions),
                std::move(flux_partitions_component_ids),
                std::move(state_partitions_component_ids));

            return std::make_tuple(result_flux_partitions, result_state_partitions);
        }

    }  // namespace detail


    namespace policy::accu_threshold {

        template<
            typename FlowDirectionElement,
            typename MaterialElement>
        using DefaultPoliciesBase = Policies<
            AllValuesWithinDomain<FlowDirectionElement, MaterialElement>,
            OutputsPolicies<
                OutputPolicies<DontMarkNoData<MaterialElement>>,
                OutputPolicies<DontMarkNoData<MaterialElement>>>,
            InputsPolicies<
                SpatialOperationInputPolicies<
                    SkipNoData<FlowDirectionElement>,
                    FlowDirectionHalo<FlowDirectionElement>>,
                SpatialOperationInputPolicies<
                    SkipNoData<MaterialElement>,
                    FillHaloWithConstantValue<MaterialElement>>,
                SpatialOperationInputPolicies<
                    SkipNoData<MaterialElement>,
                    FillHaloWithConstantValue<MaterialElement>>>>;


        template<
            typename FlowDirectionElement,
            typename MaterialElement>
        class DefaultPolicies:
            public DefaultPoliciesBase<FlowDirectionElement, MaterialElement>
        {

            public:

                using PoliciesBase = DefaultPoliciesBase<FlowDirectionElement, MaterialElement>;
                using FluxOutputPolicies = OutputPoliciesT<PoliciesBase, 0>;
                using StateOutputPolicies = OutputPoliciesT<PoliciesBase, 1>;
                using FlowDirectionInputPolicies = InputPoliciesT<PoliciesBase, 0>;
                using MaterialInputPolicies = InputPoliciesT<PoliciesBase, 1>;
                using ThresholdInputPolicies = InputPoliciesT<PoliciesBase, 2>;

                DefaultPolicies():

                    PoliciesBase{
                            DomainPolicyT<PoliciesBase>{},
                            FluxOutputPolicies{},
                            StateOutputPolicies{},
                            FlowDirectionInputPolicies{},
                            MaterialInputPolicies{FillHaloWithConstantValue<MaterialElement>{0}},
                            ThresholdInputPolicies{FillHaloWithConstantValue<MaterialElement>{0}}
                        }

                {
                }

        };


        template<
            typename FlowDirectionElement,
            typename MaterialElement>
        using DefaultValuePoliciesBase = Policies<
            // TODO Only accept non-negative material values!!!
            AllValuesWithinDomain<FlowDirectionElement, MaterialElement>,
            OutputsPolicies<
                OutputPolicies<MarkNoDataByNaN<MaterialElement>>,
                OutputPolicies<MarkNoDataByNaN<MaterialElement>>>,
            InputsPolicies<
                SpatialOperationInputPolicies<
                    DetectNoDataByValue<FlowDirectionElement>,
                    FlowDirectionHalo<FlowDirectionElement>>,
                SpatialOperationInputPolicies<
                    DetectNoDataByNaN<MaterialElement>,
                    FillHaloWithConstantValue<MaterialElement>>,
                SpatialOperationInputPolicies<
                    DetectNoDataByNaN<MaterialElement>,
                    FillHaloWithConstantValue<MaterialElement>>>>;


        template<
            typename FlowDirectionElement,
            typename MaterialElement>
        class DefaultValuePolicies:
            public DefaultValuePoliciesBase<FlowDirectionElement, MaterialElement>
        {

            public:

                using PoliciesBase = DefaultValuePoliciesBase<FlowDirectionElement, MaterialElement>;
                using FluxOutputPolicies = OutputPoliciesT<PoliciesBase, 0>;
                using StateOutputPolicies = OutputPoliciesT<PoliciesBase, 1>;
                using FlowDirectionInputPolicies = InputPoliciesT<PoliciesBase, 0>;
                using MaterialInputPolicies = InputPoliciesT<PoliciesBase, 1>;
                using ThresholdInputPolicies = InputPoliciesT<PoliciesBase, 2>;


                DefaultValuePolicies():

                    PoliciesBase{
                            DomainPolicyT<PoliciesBase>{},
                            FluxOutputPolicies{},
                            StateOutputPolicies{},
                            FlowDirectionInputPolicies{},
                            MaterialInputPolicies{FillHaloWithConstantValue<MaterialElement>{0}},
                            ThresholdInputPolicies{FillHaloWithConstantValue<MaterialElement>{0}}
                        }

                {
                }

        };

    }  // namespace policy::accu_threshold


    template<
        typename Policies,
        typename FlowDirectionElement,
        typename MaterialElement,
        Rank rank>
    std::tuple<
        PartitionedArray<MaterialElement, rank>,
        PartitionedArray<MaterialElement, rank>>
            accu_threshold(
                Policies const& policies,
                PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
                PartitionedArray<MaterialElement, rank> const& material,
                PartitionedArray<MaterialElement, rank> const& threshold)
    {
        using Localities = lue::Localities<rank>;

        Localities const& localities{flow_direction.localities()};

        // ---------------------------------------------------------------------
        // Create halo partitions around input arrays
        using FlowDirectionArray = PartitionedArray<FlowDirectionElement, rank>;
        using FlowDirectionPartitions = PartitionsT<FlowDirectionArray>;

        using MaterialArray = PartitionedArray<MaterialElement, rank>;
        using MaterialPartitions = PartitionsT<MaterialArray>;

        using Shape = ShapeT<FlowDirectionArray>;

        FlowDirectionPartitions const& flow_direction_partitions{flow_direction.partitions()};
        MaterialPartitions const& material_partitions{material.partitions()};
        MaterialPartitions const& threshold_partitions{threshold.partitions()};

        Shape min_shape{};
        std::fill(min_shape.begin(), min_shape.end(), 1);

        auto const halo_partitions = detail::halo_partitions(policies.inputs_policies(), localities,
            min_shape, flow_direction_partitions, material_partitions, threshold_partitions);

        // ---------------------------------------------------------------------
        // Calculate flow accumulation for intra-partition stream cells
        using InflowCountElement = std::uint8_t;
        using InflowCountArray = PartitionedArray<InflowCountElement, rank>;
        using InflowCountPartitions = PartitionsT<InflowCountArray>;
        using InflowCountPartition =  PartitionT<InflowCountArray>;

        using PartitionIOArray = ComponentArray<ArrayPartitionIO<
            lue::Index, rank, detail::AccuThresholdMaterial<MaterialElement>>, rank>;
        using PartitionIOComponents = ElementsT<PartitionIOArray>;
        using PartitionIOComponent = ElementT<PartitionIOArray>;

        using Action = detail::AccumulateThresholdIntraPartitionStreamCellsAction<
            Policies,
            PartitionIOComponent, InflowCountPartition,
            FlowDirectionPartitions, MaterialPartitions>;

        auto [
                partition_io_partitions,
                inflow_count_partitions,
                flux, state
            ] = detail::spawn_components<
                PartitionIOComponents, InflowCountPartitions, MaterialPartitions, MaterialPartitions>(
            policies, Action{}, localities, halo_partitions,
            flow_direction_partitions, material_partitions, threshold_partitions);

        // ---------------------------------------------------------------------
        // Calculate flow accumulation for inter-partition stream cells
        std::tie(flux, state) = detail::solve_accu_threshold(policies, localities,
            flow_direction_partitions,
            std::move(partition_io_partitions), std::move(inflow_count_partitions),
            material_partitions, threshold_partitions, std::move(flux), std::move(state));

        return std::make_tuple(
            MaterialArray{shape(flow_direction), localities, std::move(flux)},
            MaterialArray{shape(flow_direction), localities, std::move(state)});
    }


    template<
        typename FlowDirectionElement,
        typename MaterialElement,
        Rank rank>
    std::tuple<
        PartitionedArray<MaterialElement, rank>,
        PartitionedArray<MaterialElement, rank>>
            accu_threshold(
                PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
                PartitionedArray<MaterialElement, rank> const& material,
                PartitionedArray<MaterialElement, rank> const& threshold)
    {
        using Policies = policy::accu_threshold::DefaultPolicies<FlowDirectionElement, MaterialElement>;

        return accu_threshold(Policies{}, flow_direction, material, threshold);
    }

}  // namespace lue
