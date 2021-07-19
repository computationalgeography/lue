#pragma once
#include "lue/framework/algorithm/accu_info3.hpp"
#include "lue/framework/algorithm/definition/flow_accumulation3.hpp"
#include "lue/framework/algorithm/export.hpp"
#include "lue/macro.hpp"


namespace lue {
    namespace detail {

        template<
            Rank rank>
        using CellClassCommunicator = MaterialCommunicator<CellClass, rank>;


        template<
            typename Policies,
            Rank rank>
        class AccuInfoCellAccumulator
        {

            public:

                using CellClassNoDataPolicy = policy::OutputNoDataPolicy2T<policy::OutputPoliciesT<Policies, 0>>;

                using MaterialElement = CellClass;

                static_assert(std::is_same_v<policy::ElementT<CellClassNoDataPolicy>, CellClass>);

                using CellClassData = DataT<PartitionedArray<CellClass, rank>>;


                AccuInfoCellAccumulator(
                    Policies const& policies,
                    CellClassData& cell_class):

                    _ondp_cell_class{std::get<0>(policies.outputs_policies()).output_no_data_policy()},

                    _cell_class_data{cell_class}

                {
                }


                void accumulate_external_inflow(
                    [[maybe_unused]] Index const idx0,
                    [[maybe_unused]] Index const idx1)
                {
                }


                void accumulate_downstream(
                    Index const idx0_from,
                    Index const idx1_from,
                    Index const idx0_to,
                    Index const idx1_to)
                {
                    CellClass const& upstream_cell_class{_cell_class_data(idx0_from, idx1_from)};

                    CellClass& down_stream_cell_class{_cell_class_data(idx0_to, idx1_to)};

                    if(!_ondp_cell_class.is_no_data(down_stream_cell_class))
                    {
                        if(_ondp_cell_class.is_no_data(upstream_cell_class))
                        {
                            _ondp_cell_class.mark_no_data(down_stream_cell_class);
                        }
                        else
                        {
                            down_stream_cell_class = upstream_cell_class;
                        }
                    }
                }


                void accumulate_downstream(
                    CellClass const& cell_class,
                    Index const idx0_to,
                    Index const idx1_to)
                {
                    CellClass& downstream_cell_class{_cell_class_data(idx0_to, idx1_to)};

                    if(!_ondp_cell_class.is_no_data(downstream_cell_class))
                    {
                        if(_ondp_cell_class.is_no_data(cell_class))
                        {
                            _ondp_cell_class.mark_no_data(downstream_cell_class);
                        }
                        else
                        {
                            downstream_cell_class = cell_class;
                        }
                    }
                }


                CellClass const& outflow(
                    Index const idx0,
                    Index const idx1) const
                {
                    return _cell_class_data(idx0, idx1);
                }


            private:

                CellClassNoDataPolicy _ondp_cell_class;

                CellClassData& _cell_class_data;

        };


        template<
            typename Policies,
            typename FlowDirectionElement,
            Rank rank>
        ArrayPartition<CellClass, rank> accu_info3_partition(
            Policies const& policies,
            ArrayPartition<FlowDirectionElement, rank> const& flow_direction_partition,
            InflowCountCommunicator<rank> inflow_count_communicator,
            CellClassCommunicator<rank> cell_class_communicator)
        {
            using FlowDirectionPartition = ArrayPartition<FlowDirectionElement, rank>;
            using FlowDirectionData = DataT<FlowDirectionPartition>;
            using CellClassPartition = ArrayPartition<CellClass, rank>;
            using CellClassData = DataT<CellClassPartition>;
            using Offset = OffsetT<FlowDirectionPartition>;

            using CountElement = std::uint8_t;
            using InflowCountPartition = ArrayPartition<CountElement, rank>;
            using InflowCountData = DataT<InflowCountPartition>;
            using CellsIdxs = std::vector<std::array<Index, rank>>;


            auto [inflow_count_partition, input_cells_idxs_f, output_cells_idxs_f] =
                inflow_count3<Policies>(policies, flow_direction_partition, std::move(inflow_count_communicator));


            // Solve intra-partition stream cells
            hpx::future<CellClassData> cell_class_data_f;
            hpx::future<InflowCountData> inflow_count_data_f;
            {
                hpx::tie(cell_class_data_f, inflow_count_data_f, output_cells_idxs_f) =
                    hpx::split_future(hpx::dataflow(
                            hpx::launch::async,

                                [policies, cell_class_communicator](
                                    FlowDirectionPartition const& flow_direction_partition,
                                    InflowCountPartition const& inflow_count_partition,
                                    hpx::future<std::array<CellsIdxs, nr_neighbours<rank>()>>&& output_cells_idxs_f) mutable
                                {
                                    auto const flow_direction_partition_ptr{
                                        ready_component_ptr(flow_direction_partition)};
                                    FlowDirectionData const& flow_direction_data{
                                        flow_direction_partition_ptr->data()};

                                    auto const inflow_count_partition_ptr{
                                        ready_component_ptr(inflow_count_partition)};
                                    InflowCountData const& inflow_count_data{
                                        inflow_count_partition_ptr->data()};

                                    auto const& partition_shape{inflow_count_data.shape()};

                                    DataT<CellClassPartition> cell_class_data{partition_shape, 0};

                                    auto const [nr_elements0, nr_elements1] = partition_shape;

                                    auto const& indp_flow_direction =
                                        std::get<0>(policies.inputs_policies()).input_no_data_policy();
                                    auto const& ondp_cell_class =
                                        std::get<0>(policies.outputs_policies()).output_no_data_policy();

                                    // We need to copy inflow counts:
                                    // - inflow counts are used to select ridge cells
                                    // - downstream accumulation updates inflow counts
                                    InflowCountData inflow_count_data_copy{deep_copy(inflow_count_data)};

                                    using CellAccumulator = AccuInfoCellAccumulator<Policies, rank>;
                                    using Communicator = CellClassCommunicator<rank>;

                                    CellAccumulator cell_accumulator{policies, cell_class_data};
                                    auto output_cells_idxs{output_cells_idxs_f.get()};
                                    Accumulator3<CellAccumulator, Communicator> accumulator{
                                        std::move(cell_accumulator), cell_class_communicator, output_cells_idxs};

                                    for(Index idx0 = 0; idx0 < nr_elements0; ++idx0) {
                                        for(Index idx1 = 0; idx1 < nr_elements1; ++idx1)
                                        {
                                            if(indp_flow_direction.is_no_data(flow_direction_data, idx0, idx1))
                                            {
                                                // Skip cells for which we don't have a flow-direction
                                                ondp_cell_class.mark_no_data(cell_class_data, idx0, idx1);
                                            }
                                            else if(inflow_count_data(idx0, idx1) == 0)
                                            {
                                                cell_class_data(idx0, idx1) = intra_partition_stream_cell;

                                                auto [exit_cell_idxs, exit_cell_class] = accumulate3(
                                                    accumulator, idx0, idx1, flow_direction_data,
                                                    inflow_count_data_copy);

                                                cell_class_data(idx0, idx1) = ridge_cell;

                                                if(exit_cell_class == AccumulationExitCellClass::partition_output)
                                                {
                                                    auto const [exit_cell_idx0, exit_cell_idx1] = exit_cell_idxs;
                                                    cell_class_data(exit_cell_idx0, exit_cell_idx1) =
                                                        partition_output_cell;
                                                }
                                            }
                                        }
                                    }

                                    return hpx::make_tuple(
                                        std::move(cell_class_data), std::move(inflow_count_data_copy),
                                        std::move(output_cells_idxs));
                                },

                            flow_direction_partition,
                            inflow_count_partition,
                            std::move(output_cells_idxs_f)
                        ));
            }

            // Solve inter-partition stream cells
            {
                cell_class_data_f = hpx::dataflow(
                        hpx::launch::async,

                            [policies, cell_class_communicator=std::move(cell_class_communicator)](
                                FlowDirectionPartition const& flow_direction_partition,
                                hpx::future<InflowCountData>&& inflow_count_data_f,
                                hpx::shared_future<std::array<CellsIdxs, nr_neighbours<rank>()>> const& input_cells_idxs_f,
                                hpx::future<std::array<CellsIdxs, nr_neighbours<rank>()>>&& output_cells_idxs_f,
                                hpx::future<CellClassData>&& cell_class_data_f) mutable
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

                                auto const flow_direction_partition_ptr{
                                    ready_component_ptr(flow_direction_partition)};
                                FlowDirectionData const& flow_direction_data{
                                    flow_direction_partition_ptr->data()};
                                auto const& partition_shape{flow_direction_data.shape()};

                                InflowCountData inflow_count_data{inflow_count_data_f.get()};
                                CellClassData cell_class_data{cell_class_data_f.get()};

                                using CellAccumulator = AccuInfoCellAccumulator<Policies, rank>;
                                using Communicator = CellClassCommunicator<rank>;

                                CellAccumulator cell_accumulator{policies, cell_class_data};
                                auto output_cells_idxs{output_cells_idxs_f.get()};
                                Accumulator3<CellAccumulator, Communicator> accumulator{
                                    std::move(cell_accumulator), cell_class_communicator, output_cells_idxs};

                                hpx::lcos::local::mutex accu_mutex;

                                auto accumulate =
                                    [
                                        &accu_mutex,
                                        &accumulator,
                                        &flow_direction_data,
                                        &inflow_count_data,
                                        &cell_class_data
                                    ](
                                        std::array<Index, rank> const& cell_idxs,
                                        CellClass const value) mutable
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
                                            cell_class_data(idx0, idx1) = inter_partition_stream_cell;

                                            auto [exit_cell_idxs, exit_cell_class] = accumulate3(
                                                accumulator, idx0, idx1, flow_direction_data,
                                                inflow_count_data);

                                            cell_class_data(idx0, idx1) = partition_input_cell;

                                            if(exit_cell_class == AccumulationExitCellClass::partition_output)
                                            {
                                                auto const [exit_cell_idx0, exit_cell_idx1] = exit_cell_idxs;
                                                cell_class_data(exit_cell_idx0, exit_cell_idx1) =
                                                    partition_output_cell;
                                            }
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
                                                CellClass, decltype(north_idx_converter),
                                                Accumulate, rank>,
                                            std::move(cells_idxs),
                                            cell_class_communicator.receive_channel(accu::Direction::north),
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
                                                CellClass, decltype(south_idx_converter),
                                                Accumulate, rank>,
                                            std::move(cells_idxs),
                                            cell_class_communicator.receive_channel(accu::Direction::south),
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
                                                CellClass, decltype(west_idx_converter),
                                                Accumulate, rank>,
                                            std::move(cells_idxs),
                                            cell_class_communicator.receive_channel(accu::Direction::west),
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
                                                CellClass, decltype(east_idx_converter),
                                                Accumulate, rank>,
                                            std::move(cells_idxs),
                                            cell_class_communicator.receive_channel(accu::Direction::east),
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
                                                CellClass, decltype(north_west_idx_converter),
                                                Accumulate, rank>,
                                            std::move(cells_idxs),
                                            cell_class_communicator.receive_channel(accu::Direction::north_west),
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
                                                CellClass, decltype(north_east_idx_converter),
                                                Accumulate, rank>,
                                            std::move(cells_idxs),
                                            cell_class_communicator.receive_channel(accu::Direction::north_east),
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
                                                CellClass, decltype(south_east_idx_converter),
                                                Accumulate, rank>,
                                            std::move(cells_idxs),
                                            cell_class_communicator.receive_channel(accu::Direction::south_east),
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
                                                CellClass, decltype(south_west_idx_converter),
                                                Accumulate, rank>,
                                            std::move(cells_idxs),
                                            cell_class_communicator.receive_channel(accu::Direction::south_west),
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

                                // All output idxs must have been solved by now
                                lue_hpx_assert(std::all_of(output_cells_idxs.begin(), output_cells_idxs.end(),
                                    [](auto const& idxs) { return idxs.empty(); }));

                                // TODO Assert all inflow counts are zero

                                return cell_class_data;
                            },


                        flow_direction_partition,
                        inflow_count_data_f,
                        std::move(input_cells_idxs_f),
                        std::move(output_cells_idxs_f),
                        cell_class_data_f
                    );
            }

            // Once the futures of the results of the
            // inter-partition stream calculations have become ready,
            // return the result partitions.
            hpx::future<CellClassPartition> cell_class_partition_f = hpx::dataflow(
                    hpx::launch::async,

                            [](
                                FlowDirectionPartition const& flow_direction_partition,
                                hpx::future<CellClassData>&& cell_class_data_f)
                            {
                                using Server = typename CellClassPartition::Server;

                                Offset const partition_offset{
                                    ready_component_ptr(flow_direction_partition)->offset()};

                                return CellClassPartition{
                                    hpx::new_<Server>(
                                        hpx::find_here(), partition_offset, cell_class_data_f.get())};
                            },

                    flow_direction_partition,
                    std::move(cell_class_data_f)
                );

            return CellClassPartition{std::move(cell_class_partition_f)};
        }


        template<
            typename Policies,
            typename FlowDirectionElement,
            Rank rank>
        struct AccuInfoAction3:
            hpx::actions::make_action<
                decltype(&accu_info3_partition<
                    Policies, FlowDirectionElement, rank>),
                &accu_info3_partition<
                    Policies, FlowDirectionElement, rank>,
                AccuInfoAction3<
                    Policies, FlowDirectionElement, rank>>::type
        {};

    }  // namespace detail


    template<
        typename Policies,
        typename FlowDirectionElement,
        Rank rank>
    PartitionedArray<CellClass, rank> accu_info3(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, rank> const& flow_direction)
    {
        using CellClassArray = PartitionedArray<CellClass, rank>;
        using CellClassPartitions = PartitionsT<CellClassArray>;

        auto const& shape_in_partitions{flow_direction.partitions().shape()};
        Localities<rank> const& localities{flow_direction.localities()};


        using InflowCountCommunicator = detail::InflowCountCommunicator<rank>;
        using InflowCountCommunicatorArray = detail::CommunicatorArray<InflowCountCommunicator, rank>;
        using CellClassCommunicator = detail::CellClassCommunicator<rank>;
        using CellClassCommunicatorArray = detail::CommunicatorArray<CellClassCommunicator, rank>;

        InflowCountCommunicatorArray inflow_count_communicators{"/lue/accu_info3/inflow_count/", localities};
        CellClassCommunicatorArray cell_class_communicators{"/lue/accu_info3/", localities};


        CellClassPartitions cell_class_partitions{shape_in_partitions};
        detail::AccuInfoAction3<Policies, FlowDirectionElement, rank> action{};
        Count const nr_partitions{nr_elements(shape_in_partitions)};

        for(Index p = 0; p < nr_partitions; ++p)
        {
            cell_class_partitions[p] = hpx::async(
                action, localities[p], policies,
                flow_direction.partitions()[p],
                inflow_count_communicators[p], cell_class_communicators[p]);
        }


        hpx::when_all(cell_class_partitions.begin(), cell_class_partitions.end(),
            [
                inflow_count_communicators=std::move(inflow_count_communicators),
                cell_class_communicators=std::move(cell_class_communicators)
            ]([[maybe_unused]] auto&& partitions) mutable
            {
                auto f1{inflow_count_communicators.unregister()};
                auto f2{cell_class_communicators.unregister()};

                hpx::wait_all(f1, f2);
            });


        return CellClassArray{flow_direction.shape(), localities, std::move(cell_class_partitions)};
    }

}  // namespace lue


#define LUE_INSTANTIATE_ACCU_INFO3(                                  \
    Policies, FlowDirectionElement)                                  \
                                                                     \
    template LUE_FA_EXPORT                                           \
    PartitionedArray<CellClass, 2> accu_info3<                       \
            ArgumentType<void(Policies)>, FlowDirectionElement, 2>(  \
        ArgumentType<void(Policies)> const&,                         \
        PartitionedArray<FlowDirectionElement, 2> const&);
