#pragma once
#include "lue/framework/algorithm/detail/accumulate.hpp"
#include "lue/framework/algorithm/detail/promise.hpp"
#include "lue/framework/algorithm/component/array_partition_io.hpp"
#include "lue/framework/algorithm/inflow_count.hpp"
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/algorithm/type_traits.hpp"
#include "lue/framework/core/assert.hpp"
#include "lue/framework/core/component/component_array.hpp"
#include "lue/framework/core/serialize/array.hpp"


namespace lue {
    namespace detail {
        namespace accu {

            template<
                typename InputNoDataPolicy,
                typename OutputNoDataPolicy,
                typename InputMaterialElement,
                typename OutputMaterialElement>
            class CellAccumulator
            {

                public:

                    CellAccumulator(
                        InputNoDataPolicy const& indp,
                        OutputNoDataPolicy const& ondp):

                        _indp{indp},
                        _ondp{ondp}

                    {
                    }


                    void accumulate_input(
                        InputMaterialElement const& input_material_element,
                        OutputMaterialElement& output_material_element) const
                    {
                        if(!_ondp.is_no_data(output_material_element))
                        {
                            if(_indp.is_no_data(input_material_element))
                            {
                                _ondp.mark_no_data(output_material_element);
                            }
                            else
                            {
                                output_material_element += input_material_element;
                            }
                        }
                    }


                    void accumulate_downstream(
                        OutputMaterialElement const& upstream_material_element,
                        OutputMaterialElement& downstream_material_element) const
                    {
                        if(!_ondp.is_no_data(downstream_material_element))
                        {
                            if(_ondp.is_no_data(upstream_material_element))
                            {
                                _ondp.mark_no_data(downstream_material_element);
                            }
                            else
                            {
                                downstream_material_element += upstream_material_element;
                            }
                        }
                    }


                private:

                    InputNoDataPolicy const& _indp;

                    OutputNoDataPolicy const& _ondp;

            };

        }  // namespace accu


        template<
            typename Policies,
            typename PartitionIOComponent,
            typename InflowCountPartition,
            typename FlowDirectionPartitions,
            typename MaterialPartitions>
        hpx::util::tuple<PartitionIOComponent, InflowCountPartition, PartitionT<MaterialPartitions>>
            accumulate_intra_partition_stream_cells(
                Policies const& policies,
                OffsetT<PartitionT<FlowDirectionPartitions>> const& partition_offset,
                FlowDirectionPartitions const& flow_direction_partitions,
                MaterialPartitions const& material_partitions)
        {
            using FlowDirectionPartition = PartitionT<FlowDirectionPartitions>;
            using FlowDirectionData = DataT<FlowDirectionPartition>;
            using FlowDirectionElement = ElementT<FlowDirectionPartitions>;

            using MaterialPartition = PartitionT<MaterialPartitions>;
            using MaterialData = DataT<MaterialPartition>;
            using MaterialElement = ElementT<MaterialPartitions>;

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

            // Once all input data has arrived...
            return hpx::split_future(hpx::dataflow(
                hpx::launch::async,
                hpx::util::unwrapping(

                        [policies, partition_offset](
                            Offset offset,
                            lue::Array<FlowDirectionData, rank<FlowDirectionData>> const& flow_direction_data,
                            lue::Array<MaterialData, rank<MaterialData>> const& input_material_data)
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

                            using MaterialInputPolicies = policy::InputPoliciesT<Policies, 1>;
                            MaterialInputPolicies material_input_policies{
                                std::get<1>(policies.inputs_policies())};

                            using MaterialOutputPolicies = policy::OutputPoliciesT<Policies, 0>;
                            MaterialOutputPolicies material_output_policies{
                                std::get<0>(policies.outputs_policies())};

                            auto const& indp_flow_direction =
                                flow_direction_input_policies.input_no_data_policy();
                            auto const& indp_material = material_input_policies.input_no_data_policy();
                            auto const& ondp_output_material =
                                material_output_policies.output_no_data_policy();

                            using CellAccumulator = accu::CellAccumulator<
                                decltype(indp_material), decltype(ondp_output_material),
                                MaterialElement, MaterialElement>;
                            using Accumulator =
                                detail::Accumulator<PartitionIOData, MaterialData, CellAccumulator>;

                            using Shape = ShapeT<InflowCountData>;
                            Shape const partition_shape{inflow_count_data.shape()};

                            PartitionIOData partition_io_data{partition_shape, std::move(input_cell_idxs)};
                            MaterialData output_material_data{partition_shape, 0};

                            Accumulator accumulator{
                                partition_io_data, input_material_data(1, 1),
                                CellAccumulator{indp_material, ondp_output_material}, output_material_data};

                            auto const [nr_elements0, nr_elements1] = partition_shape;

                            InflowCountData inflow_count_data_copy{deep_copy(inflow_count_data)};

                            for(Index idx0 = 0; idx0 < nr_elements0; ++idx0) {
                                for(Index idx1 = 0; idx1 < nr_elements1; ++idx1)
                                {
                                    if(indp_flow_direction.is_no_data(flow_direction_data(1, 1), idx0, idx1))
                                    {
                                        // Skip cells for which we don't have a flow-direction
                                        ondp_output_material.mark_no_data(output_material_data, idx0, idx1);
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
                                        hpx::find_here(), offset, std::move(output_material_data)}
                                );
                        }

                    ),
                std::move(offset),
                when_all_get(std::move(flow_direction_data)),
                when_all_get(std::move(input_material_data))));
        }


        template<
            typename Policies,
            typename PartitionIOComponent,
            typename InflowCountPartition,
            typename FlowDirectionPartitions,
            typename MaterialPartitions>
        struct AccumulateIntraPartitionStreamCellsAction:
            hpx::actions::make_action<
                decltype(&accumulate_intra_partition_stream_cells<
                    Policies, PartitionIOComponent, InflowCountPartition,
                    FlowDirectionPartitions, MaterialPartitions>),
                &accumulate_intra_partition_stream_cells<
                    Policies, PartitionIOComponent, InflowCountPartition,
                    FlowDirectionPartitions, MaterialPartitions>,
                AccumulateIntraPartitionStreamCellsAction<
                    Policies, PartitionIOComponent, InflowCountPartition,
                    FlowDirectionPartitions, MaterialPartitions>>::type
        {};


        template<
            typename Policies,
            typename FlowDirectionPartition,
            typename PartitionIOComponent,
            typename InflowCountPartition,
            typename MaterialPartition>
        void accumulate_inter_partition_stream_cells(
            Policies const& policies,
            FlowDirectionPartition const& flow_direction_partition,
            InflowCountPartition const& inflow_count_partition,
            Array<PartitionIOComponent, 2> io_components,
            typename PartitionIOComponent::PartitionOffsets const& offsets,
            MaterialPartition const& material_partition,
            MaterialPartition const& flux_partition)
        {
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

            auto flux_partition_ptr{detail::ready_component_ptr(flux_partition)};
            auto& flux_partition_server{*flux_partition_ptr};
            auto flux_data{flux_partition_server.data()};

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

            auto const& ondp_output_material =
                std::get<0>(policies.outputs_policies()).output_no_data_policy();
            auto const& indp_material =
                std::get<1>(policies.inputs_policies()).input_no_data_policy();

            using PartitionIOData = typename PartitionIOComponent::Data;

            using CellAccumulator = accu::CellAccumulator<
                decltype(indp_material), decltype(ondp_output_material), MaterialElement, MaterialElement>;
            using Accumulator = detail::Accumulator<PartitionIOData, MaterialData, CellAccumulator>;

            Accumulator accumulator{partition_io_data, material_data,
                CellAccumulator{indp_material, ondp_output_material}, flux_data};

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

                    accumulator.cell_accumulator().accumulate_downstream(material, flux_data(idx0, idx1));

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
        struct AccumulateInterPartitionStreamCellsAction:
            hpx::actions::make_action<
                decltype(&accumulate_inter_partition_stream_cells<
                    Policies, FlowDirectionPartition, PartitionIOComponent, InflowCountPartition,
                    MaterialPartition>),
                &accumulate_inter_partition_stream_cells<
                    Policies, FlowDirectionPartition, PartitionIOComponent, InflowCountPartition,
                    MaterialPartition>,
                AccumulateInterPartitionStreamCellsAction<
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
        void solve_accu_array(
            Policies const& policies,
            Localities const& localities,
            FlowDirectionPartitions const& flow_direction_partitions,
            PartitionIOComponents&& partition_io_partitions,
            InflowCountPartitions&& inflow_count_partitions,
            MaterialPartitions const& material_partitions,
            MaterialPartitions&& flux_partitions,
            IDPromiseArray<rank<MaterialPartitions>>&& flux_promises)
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
            using MaterialPartition = ComponentT<MaterialPartitions>;

            using Shape = ShapeT<MaterialPartitions>;

            Shape const& shape_in_partitions{flux_partitions.shape()};
            Count const nr_partitions{nr_elements(shape_in_partitions)};
            auto const [ extent0, extent1 ] = shape_in_partitions;

            Count nr_iterations{0};
            std::vector<hpx::future<void>> accu_futures;

            using Action = detail::AccumulateInterPartitionStreamCellsAction<
                Policies,
                FlowDirectionPartition, PartitionIOComponent, InflowCountPartition,
                MaterialPartition>;

            Array<bool, 2> ready_partitions{shape_in_partitions, false};
            Count nr_ready_partitions{0};
            Action action;

            while(true)
            {
                // Determine which partitions are part of the accumulation front
                auto [ solved_partitions, upstream_partition_offsets ] =
                    flow_accumulation_front(partition_io_partitions, ready_partitions);

                accu_futures.clear();

                for(Index idx0 = 0; idx0 < extent0; ++idx0) {
                    for(Index idx1 = 0; idx1 < extent1; ++idx1)
                    {
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

                                accu_futures.push_back(hpx::async(action, localities(idx0, idx1),
                                    policies,
                                    flow_direction_partitions(idx0, idx1), inflow_count_partitions(idx0, idx1),
                                    io_components, offsets,
                                    material_partitions(idx0, idx1),
                                    flux_partitions(idx0, idx1)));
                            }
                        }
                    }
                }

                Count nr_ready_partitions_new =
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
        MaterialPartitions solve_accu(
            Policies const& policies,
            Localities const& localities,
            FlowDirectionPartitions const& flow_direction_partitions,
            PartitionIOComponents&& partition_io_partitions,
            InflowCountPartitions&& inflow_count_partitions,
            MaterialPartitions const& material_partitions,
            MaterialPartitions&& current_flux_partitions)
        {
            // This function must return results as quickly as possible

            // Create and a collection of array partition clients. Each of
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

            // For each partition a client instance, that containing a
            // future to a partition server instance ID
            MaterialPartitions result_flux_partitions{shape_in_partitions};

            Count const nr_partitions{nr_elements(shape_in_partitions)};

            for(Index p = 0; p < nr_partitions; ++p)
            {
                // Connect the array with promises with the array of futures
                result_flux_partitions[p] = MaterialPartition{flux_partitions_component_ids[p].get_future()};
                lue_hpx_assert(result_flux_partitions[p].valid());
            }

            // Asynchronously start the calculations that will
            // result in the global flow accumulation result.
            // During the calculations, the promises of the individual
            // partition server instance IDs will be fulfilled, as soon
            // as possible. Fire and forget.
            hpx::apply(
                solve_accu_array<Policies, Localities,
                    FlowDirectionPartitions, PartitionIOComponents, InflowCountPartitions,
                    MaterialPartitions>,
                policies,
                localities,
                flow_direction_partitions,
                std::move(partition_io_partitions),
                std::move(inflow_count_partitions),
                material_partitions,
                std::move(current_flux_partitions),
                std::move(flux_partitions_component_ids));

            return result_flux_partitions;
        }

    }  // namespace detail


    namespace policy::accu {

        template<
            typename FlowDirectionElement,
            typename MaterialElement>
        using DefaultPoliciesBase = Policies<
            AllValuesWithinDomain<FlowDirectionElement, MaterialElement>,
            OutputsPolicies<
                OutputPolicies<DontMarkNoData<MaterialElement>>>,
            InputsPolicies<
                SpatialOperationInputPolicies<
                    SkipNoData<FlowDirectionElement>,
                    FlowDirectionHalo<FlowDirectionElement>>,
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
                using MaterialOutputPolicies = OutputPoliciesT<PoliciesBase, 0>;
                using FlowDirectionInputPolicies = InputPoliciesT<PoliciesBase, 0>;
                using MaterialInputPolicies = InputPoliciesT<PoliciesBase, 1>;

                DefaultPolicies():

                    PoliciesBase{
                            DomainPolicyT<PoliciesBase>{},
                            MaterialOutputPolicies{},
                            FlowDirectionInputPolicies{},
                            MaterialInputPolicies{FillHaloWithConstantValue<MaterialElement>{0}}
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
                OutputPolicies<DefaultOutputNoDataPolicy<MaterialElement>>>,
            InputsPolicies<
                SpatialOperationInputPolicies<
                    DetectNoDataByValue<FlowDirectionElement>,
                    FlowDirectionHalo<FlowDirectionElement>>,
                SpatialOperationInputPolicies<
                    DefaultInputNoDataPolicy<MaterialElement>,
                    FillHaloWithConstantValue<MaterialElement>>>>;


        template<
            typename FlowDirectionElement,
            typename MaterialElement>
        class DefaultValuePolicies:
            public DefaultValuePoliciesBase<FlowDirectionElement, MaterialElement>
        {

            public:

                using PoliciesBase = DefaultValuePoliciesBase<FlowDirectionElement, MaterialElement>;
                using MaterialOutputPolicies = OutputPoliciesT<PoliciesBase, 0>;
                using FlowDirectionInputPolicies = InputPoliciesT<PoliciesBase, 0>;
                using MaterialInputPolicies = InputPoliciesT<PoliciesBase, 1>;

                DefaultValuePolicies():

                    PoliciesBase{
                            DomainPolicyT<PoliciesBase>{},
                            MaterialOutputPolicies{},
                            FlowDirectionInputPolicies{},
                            MaterialInputPolicies{FillHaloWithConstantValue<MaterialElement>{0}}
                        }

                {
                }

        };

    }  // namespace policy::accu


    template<
        typename Policies,
        typename FlowDirectionElement,
        typename MaterialElement,
        Rank rank>
    PartitionedArray<MaterialElement, rank> accu(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
        PartitionedArray<MaterialElement, rank> const& material)
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

        Shape min_shape{};
        std::fill(min_shape.begin(), min_shape.end(), 1);

        auto const halo_partitions = detail::halo_partitions(policies.inputs_policies(), localities,
            min_shape, flow_direction_partitions, material_partitions);

        // ---------------------------------------------------------------------
        // Calculate flow accumulation for intra-partition stream cells
        using InflowCountElement = std::uint8_t;
        using InflowCountArray = PartitionedArray<InflowCountElement, rank>;
        using InflowCountPartitions = PartitionsT<InflowCountArray>;
        using InflowCountPartition =  PartitionT<InflowCountArray>;

        using PartitionIOArray = ComponentArray<ArrayPartitionIO<lue::Index, rank, MaterialElement>, rank>;
        using PartitionIOComponents = ElementsT<PartitionIOArray>;
        using PartitionIOComponent = ElementT<PartitionIOArray>;

        using Action = detail::AccumulateIntraPartitionStreamCellsAction<
            Policies,
            PartitionIOComponent, InflowCountPartition,
            FlowDirectionPartitions, MaterialPartitions>;

        auto [
                partition_io_partitions,
                inflow_count_partitions,
                flux
            ] = detail::spawn_components<
                PartitionIOComponents, InflowCountPartitions, MaterialPartitions>(
            policies, Action{}, localities, halo_partitions,
            flow_direction_partitions, material_partitions);

        // ---------------------------------------------------------------------
        // Calculate flow accumulation for inter-partition stream cells
        flux = detail::solve_accu(policies, localities,
            flow_direction_partitions,
            std::move(partition_io_partitions), std::move(inflow_count_partitions),
            material_partitions, std::move(flux));

        return MaterialArray{shape(flow_direction), localities, std::move(flux)};
    }


    template<
        typename FlowDirectionElement,
        typename MaterialElement,
        Rank rank>
    PartitionedArray<MaterialElement, rank> accu(
        PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
        PartitionedArray<MaterialElement, rank> const& material)
    {
        using Policies = policy::accu::DefaultPolicies<FlowDirectionElement, MaterialElement>;

        return accu(Policies{}, flow_direction, material);
    }

}  // namespace lue
