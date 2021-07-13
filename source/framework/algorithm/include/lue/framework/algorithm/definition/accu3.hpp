#pragma once
#include "lue/framework/algorithm/accu3.hpp"
#include "lue/framework/algorithm/definition/flow_accumulation3.hpp"
#include "lue/framework/algorithm/export.hpp"
#include "lue/macro.hpp"


namespace lue {
    namespace detail {

        template<
            typename InflowNoDataPolicy,
            typename OutflowNoDataPolicy,
            Rank rank>
        class AccuCellAccumulator
        {

            public:

                using MaterialElement = policy::ElementT<InflowNoDataPolicy>;
                static_assert(std::is_same_v<policy::ElementT<InflowNoDataPolicy>, MaterialElement>);
                static_assert(std::is_same_v<policy::ElementT<OutflowNoDataPolicy>, MaterialElement>);

                using MaterialData = DataT<PartitionedArray<MaterialElement, rank>>;

                AccuCellAccumulator(
                    InflowNoDataPolicy const& indp_inflow,
                    OutflowNoDataPolicy const& ondp_outflow,
                    MaterialData const& inflow,
                    MaterialData& outflow):

                    _indp_inflow{indp_inflow},
                    _ondp_outflow{ondp_outflow},
                    _inflow{inflow},
                    _outflow{outflow}

                {
                }


                void accumulate_external_inflow(
                    Index const idx0,
                    Index const idx1)
                {
                    MaterialElement const& inflow{_inflow(idx0, idx1)};

                    MaterialElement& outflow{_outflow(idx0, idx1)};

                    if(!_ondp_outflow.is_no_data(outflow))
                    {
                        if(_indp_inflow.is_no_data(inflow))
                        {
                            _ondp_outflow.mark_no_data(outflow);
                        }
                        else
                        {
                            // TODO Domain check

                            // Now we know the final, total amount
                            // of inflow that enters this cell
                            outflow += inflow;
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

                    MaterialElement& inflow{_outflow(idx0_to, idx1_to)};

                    if(!_ondp_outflow.is_no_data(inflow))
                    {
                        if(_ondp_outflow.is_no_data(outflow))
                        {
                            _ondp_outflow.mark_no_data(inflow);
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

                    if(!_ondp_outflow.is_no_data(inflow))
                    {
                        if(_ondp_outflow.is_no_data(outflow))
                        {
                            _ondp_outflow.mark_no_data(inflow);
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

                OutflowNoDataPolicy _ondp_outflow;

                MaterialData const& _inflow;  // External inflow

                MaterialData& _outflow;  // Upstream inflow, outflow

        };


        template<
            typename Policies,
            typename FlowDirectionElement,
            typename MaterialElement,
            Rank rank>
        ArrayPartition<MaterialElement, rank> accu3_partition(
            Policies const& policies,
            ArrayPartition<FlowDirectionElement, rank> const& flow_direction_partition,
            ArrayPartition<MaterialElement, rank> const& external_inflow_partition,
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

            // Solve intra-partition stream cells
            hpx::future<MaterialData> outflow_data_f;
            hpx::future<InflowCountData> inflow_count_data_f;
            {
                // Once the input partitions and inflow_count_data
                // are ready, spawn a task that will calculate the flow
                // accumulation for the intra-partition stream
                // cells. Whenever material reaches the border of the
                // partition, it is sent to the corresponding task
                // managing the neighbouring partition.
                hpx::tie(outflow_data_f, inflow_count_data_f) =
                    hpx::split_future(hpx::dataflow(
                            hpx::launch::async,

                                [policies, material_communicator](
                                    FlowDirectionPartition const& flow_direction_partition,
                                    MaterialPartition const& external_inflow_partition,
                                    InflowCountPartition const& inflow_count_partition) mutable
                                {
                                    AnnotateFunction annotation{"intra_partition_stream"};
                                    auto const flow_direction_partition_ptr{
                                        ready_component_ptr(flow_direction_partition)};
                                    FlowDirectionData const& flow_direction_data{
                                        flow_direction_partition_ptr->data()};

                                    auto const external_inflow_partition_ptr{
                                        ready_component_ptr(external_inflow_partition)};
                                    MaterialData const& external_inflow_data{
                                        external_inflow_partition_ptr->data()};

                                    auto const inflow_count_partition_ptr{
                                        ready_component_ptr(inflow_count_partition)};
                                    InflowCountData const& inflow_count_data{
                                        inflow_count_partition_ptr->data()};

                                    auto const& partition_shape{inflow_count_data.shape()};

                                    DataT<MaterialPartition> outflow_data{partition_shape, 0};

                                    auto const [nr_elements0, nr_elements1] = partition_shape;

                                    auto const& indp_flow_direction =
                                        std::get<0>(policies.inputs_policies()).input_no_data_policy();
                                    auto const& indp_inflow =
                                        std::get<1>(policies.inputs_policies()).input_no_data_policy();
                                    auto const& ondp_outflow =
                                        std::get<0>(policies.outputs_policies()).output_no_data_policy();

                                    // We need to copy inflow counts:
                                    // - inflow counts are used to select ridge cells
                                    // - downstream accumulation updates inflow counts
                                    InflowCountData inflow_count_data_copy{deep_copy(inflow_count_data)};

                                    using CellAccumulator = AccuCellAccumulator<
                                        decltype(indp_inflow), decltype(ondp_outflow), rank>;
                                    using Communicator = MaterialCommunicator<MaterialElement, rank>;

                                    CellAccumulator cell_accumulator{
                                        indp_inflow, ondp_outflow, external_inflow_data, outflow_data};
                                    Accumulator3<CellAccumulator, Communicator> accumulator{
                                        std::move(cell_accumulator), material_communicator};

                                    for(Index idx0 = 0; idx0 < nr_elements0; ++idx0) {
                                        for(Index idx1 = 0; idx1 < nr_elements1; ++idx1)
                                        {
                                            if(indp_flow_direction.is_no_data(flow_direction_data, idx0, idx1))
                                            {
                                                // Skip cells for which we don't have a flow-direction
                                                ondp_outflow.mark_no_data(outflow_data, idx0, idx1);
                                            }
                                            else if(inflow_count_data(idx0, idx1) == 0)
                                            {
                                                accumulate3(
                                                    accumulator, idx0, idx1, flow_direction_data,
                                                    inflow_count_data_copy);
                                            }
                                        }
                                    }

                                    return hpx::make_tuple(
                                        std::move(outflow_data), std::move(inflow_count_data_copy));
                                },

                            flow_direction_partition,
                            external_inflow_partition,
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
                outflow_data_f = hpx::dataflow(
                        hpx::launch::async,

                            [policies, material_communicator=std::move(material_communicator)](
                                FlowDirectionPartition const& flow_direction_partition,
                                MaterialPartition const& external_inflow_partition,
                                hpx::future<InflowCountData>&& inflow_count_data_f,
                                hpx::shared_future<std::array<CellsIdxs, nr_neighbours<rank>()>> const& input_cells_idxs_f,
                                hpx::future<MaterialData>&& outflow_data_f) mutable
                            {
                                AnnotateFunction annotation{"inter_partition_stream"};

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

                                InflowCountData inflow_count_data{inflow_count_data_f.get()};
                                MaterialData outflow_data{outflow_data_f.get()};

                                auto const& indp_inflow =
                                    std::get<1>(policies.inputs_policies()).input_no_data_policy();
                                auto const& ondp_outflow =
                                    std::get<0>(policies.outputs_policies()).output_no_data_policy();

                                using CellAccumulator = AccuCellAccumulator<
                                    decltype(indp_inflow), decltype(ondp_outflow), rank>;
                                using Communicator = MaterialCommunicator<MaterialElement, rank>;

                                CellAccumulator cell_accumulator{
                                    indp_inflow, ondp_outflow, external_inflow_data, outflow_data};
                                Accumulator3<CellAccumulator, Communicator> accumulator{
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
                                            detail::accumulate3(
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

                                return outflow_data;
                            },


                        flow_direction_partition,
                        external_inflow_partition,
                        inflow_count_data_f,
                        input_cells_idxs_f,
                        outflow_data_f
                    );
            }

            // Once the futures of the results of the
            // inter-partition stream calculations have become ready,
            // return the result partitions.
            hpx::future<MaterialPartition> outflow_partition_f = hpx::dataflow(
                    hpx::launch::async,

                            [](
                                FlowDirectionPartition const& flow_direction_partition,
                                hpx::future<MaterialData>&& outflow_data_f)
                            {
                                AnnotateFunction annotation{"create_result_partitions"};
                                using Server = typename MaterialPartition::Server;

                                Offset const partition_offset{
                                    ready_component_ptr(flow_direction_partition)->offset()};

                                return MaterialPartition{
                                    hpx::new_<Server>(
                                        hpx::find_here(), partition_offset, outflow_data_f.get())};
                            },

                    flow_direction_partition,
                    std::move(outflow_data_f)
                );

            return MaterialPartition{std::move(outflow_partition_f)};
        }


        template<
            typename Policies,
            typename FlowDirectionElement,
            typename MaterialElement,
            Rank rank>
        struct AccuAction3:
            hpx::actions::make_action<
                decltype(&accu3_partition<
                    Policies, FlowDirectionElement, MaterialElement, rank>),
                &accu3_partition<
                    Policies, FlowDirectionElement, MaterialElement, rank>,
                AccuAction3<
                    Policies, FlowDirectionElement, MaterialElement, rank>>::type
        {};

    }  // namespace detail


    template<
        typename Policies,
        typename FlowDirectionElement,
        typename MaterialElement,
        Rank rank>
    PartitionedArray<MaterialElement, rank> accu3(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
        PartitionedArray<MaterialElement, rank> const& external_inflow)
    {
        AnnotateFunction annotation{"accu_meh"};

        using MaterialArray = PartitionedArray<MaterialElement, rank>;
        using MaterialPartitions = PartitionsT<MaterialArray>;

        auto const& shape_in_partitions{flow_direction.partitions().shape()};
        Localities<rank> const& localities{flow_direction.localities()};


        // Create communicators used in solving the flow accumulation
        using InflowCountCommunicator = detail::InflowCountCommunicator<rank>;
        using InflowCountCommunicatorArray = detail::CommunicatorArray<InflowCountCommunicator, rank>;
        using MaterialCommunicator = detail::MaterialCommunicator<MaterialElement, rank>;
        using MaterialCommunicatorArray = detail::CommunicatorArray<MaterialCommunicator, rank>;

        InflowCountCommunicatorArray inflow_count_communicators{"/lue/accu3/inflow_count/", localities};
        MaterialCommunicatorArray material_communicators{"/lue/accu3/", localities};


        // For each partition, spawn a task that will solve the
        // flow accumulation for the partition.
        MaterialPartitions outflow_partitions{shape_in_partitions};
        detail::AccuAction3<Policies, FlowDirectionElement, MaterialElement, rank> action{};
        Count const nr_partitions{nr_elements(shape_in_partitions)};

        for(Index p = 0; p < nr_partitions; ++p)
        {
            outflow_partitions[p] = hpx::async(
                hpx::util::annotated_function(action, "accu"), localities[p], policies,
                flow_direction.partitions()[p],
                external_inflow.partitions()[p],
                inflow_count_communicators[p], material_communicators[p]);
        }


        // The communicators used by the tasks calculating the flow
        // accumulation result must be kept alive until these results
        // are ready. We do this by attaching a continuation,
        // passing in the communicators. Once the results are ready,
        // these communicators go out of scope. Once they do, free up
        // AGAS resources. Note that component clients are
        // reference-counted.
        hpx::when_all(outflow_partitions.begin(), outflow_partitions.end(),
            [
                inflow_count_communicators=std::move(inflow_count_communicators),
                material_communicators=std::move(material_communicators)
            ]([[maybe_unused]] auto&& partitions)
            {
                HPX_UNUSED(inflow_count_communicators);
                HPX_UNUSED(material_communicators);
            });


        return MaterialArray{flow_direction.shape(), localities, std::move(outflow_partitions)};
    }

}  // namespace lue


#define LUE_INSTANTIATE_ACCU3(                                                        \
    Policies, FlowDirectionElement, MaterialElement)                                  \
                                                                                      \
    template LUE_FA_EXPORT                                                            \
    PartitionedArray<MaterialElement, 2> accu3<                                       \
            ArgumentType<void(Policies)>, FlowDirectionElement, MaterialElement, 2>(  \
        ArgumentType<void(Policies)> const&,                                          \
        PartitionedArray<FlowDirectionElement, 2> const&,                             \
        PartitionedArray<MaterialElement, 2> const&);
