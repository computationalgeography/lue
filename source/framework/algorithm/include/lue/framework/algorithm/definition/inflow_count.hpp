#pragma once
#include "lue/framework/algorithm/detail/communicator_array.hpp"
#include "lue/framework/algorithm/detail/idx_converter.hpp"
#include "lue/framework/algorithm/detail/inflow_count.hpp"  // inflow_count_partition_data
#include "lue/framework/algorithm/inflow_count.hpp"
#include "lue/framework/algorithm/routing_operation_export.hpp"
#include "lue/framework/core/serialize_resource_usage.hpp"
#include "lue/macro.hpp"


namespace lue {
    namespace detail {

        template<Rank rank>
        using InflowCountCommunicator = Communicator<std::vector<Index>, rank>;


        template<typename T, typename IdxConverter, Rank rank>
        auto monitor_cell_idx_inputs(
            hpx::distributed::channel<T> const& channel,
            IdxConverter const& idx_to_idxs,
            [[maybe_unused]] Shape<Index, rank> const& partition_shape)
            -> std::vector<std::array<Index, rank>>
        {
            AnnotateFunction annotation{"monitor_cell_idx_inputs"};

            std::vector<std::array<Index, rank>> cells_idxs{};

            if (channel)
            {
                // Expecting a single, possibly empty, collection
                hpx::future<std::vector<Index>> idxs_f{channel.get(hpx::launch::async)};
                idxs_f.wait();
                lue_hpx_assert(!idxs_f.has_exception());

                std::vector<Index> idxs{idxs_f.get()};

                cells_idxs.reserve(idxs.size());
                std::transform(idxs.begin(), idxs.end(), std::back_inserter(cells_idxs), idx_to_idxs);
            }

#ifndef NDEBUG
            auto const [extent0, extent1] = partition_shape;

            for (auto const& cell_idxs : cells_idxs)
            {
                auto const [idx0, idx1] = cell_idxs;

                lue_hpx_assert(idx0 >= 0);
                lue_hpx_assert(idx0 < extent0);
                lue_hpx_assert(idx1 >= 0);
                lue_hpx_assert(idx1 < extent1);
                lue_hpx_assert((idx0 == 0 || idx0 == extent0 - 1) || (idx1 == 0 || idx1 == extent1 - 1));
            }
#endif

            return cells_idxs;
        }


        template<typename Policies, typename FlowDirectionElement, Rank rank>
        auto connectivity_ready(
            Policies const& policies,
            ArrayPartition<FlowDirectionElement, rank> const& flow_direction_partition,
            InflowCountCommunicator<rank>&& communicator)
            -> hpx::tuple<
                hpx::future<std::array<std::vector<std::array<Index, rank>>, nr_neighbours<rank>()>>,
                std::array<std::vector<std::array<Index, rank>>, nr_neighbours<rank>()>>
        {
            lue_hpx_assert(flow_direction_partition.is_ready());

            using CellsIdxs = std::vector<std::array<Index, rank>>;

            auto const flow_direction_data = flow_direction_partition.data(hpx::launch::sync);
            auto const [extent0, extent1] = flow_direction_data.shape();
            Count const nr_neighbours{detail::nr_neighbours<rank>()};

            // For each partition border:
            // - For each cell that flows into a neighbouring partition
            //     - Determine the neighbouring partition (n, ne, e, ...)
            //     - Determine 1D input cell index in neighbouring partition
            //     - Store indices of output cell
            // - Send 1D idxs of receiving cells to task managing
            //     neighbouring partition, if any

            std::array<CellsIdxs, nr_neighbours> output_cells_idxs{};
            for (auto& cells_idxs : output_cells_idxs)
            {
                cells_idxs.reserve(20);  // Prevent first few reallocations
            }

            {
                std::array<std::vector<Index>, nr_neighbours> input_cell_idxs{};
                for (auto& cell_idxs : input_cell_idxs)
                {
                    cell_idxs.reserve(20);  // Prevent first few reallocations
                }

                auto const& indp{std::get<0>(policies.inputs_policies()).input_no_data_policy()};
                Index idx0, idx1;
                FlowDirectionElement flow_direction;

                if (extent1 > 2)
                {
                    // North side, excluding north-west and north-east corners
                    idx0 = 0;

                    for (idx1 = 1; idx1 < extent1 - 1; ++idx1)
                    {
                        if (!indp.is_no_data(flow_direction_data, idx0, idx1))
                        {
                            flow_direction = flow_direction_data(idx0, idx1);

                            if (flow_direction == north_west<FlowDirectionElement>)
                            {
                                output_cells_idxs[accu::Direction::north].push_back({idx0, idx1});
                                input_cell_idxs[accu::Direction::north].push_back(idx1 - 1);
                            }
                            else if (flow_direction == north<FlowDirectionElement>)
                            {
                                output_cells_idxs[accu::Direction::north].push_back({idx0, idx1});
                                input_cell_idxs[accu::Direction::north].push_back(idx1);
                            }
                            else if (flow_direction == north_east<FlowDirectionElement>)
                            {
                                output_cells_idxs[accu::Direction::north].push_back({idx0, idx1});
                                input_cell_idxs[accu::Direction::north].push_back(idx1 + 1);
                            }
                        }
                    }

                    // South side, excluding south-west and south-east corners
                    idx0 = extent0 - 1;

                    for (idx1 = 1; idx1 < extent1 - 1; ++idx1)
                    {
                        if (!indp.is_no_data(flow_direction_data, idx0, idx1))
                        {
                            flow_direction = flow_direction_data(idx0, idx1);

                            if (flow_direction == south_west<FlowDirectionElement>)
                            {
                                output_cells_idxs[accu::Direction::south].push_back({idx0, idx1});
                                input_cell_idxs[accu::Direction::south].push_back(idx1 - 1);
                            }
                            else if (flow_direction == south<FlowDirectionElement>)
                            {
                                output_cells_idxs[accu::Direction::south].push_back({idx0, idx1});
                                input_cell_idxs[accu::Direction::south].push_back(idx1);
                            }
                            else if (flow_direction == south_east<FlowDirectionElement>)
                            {
                                output_cells_idxs[accu::Direction::south].push_back({idx0, idx1});
                                input_cell_idxs[accu::Direction::south].push_back(idx1 + 1);
                            }
                        }
                    }
                }

                if (extent0 > 2)
                {
                    // West side, excluding north-west and south-west corners
                    idx1 = 0;

                    for (idx0 = 1; idx0 < extent0 - 1; ++idx0)
                    {
                        if (!indp.is_no_data(flow_direction_data, idx0, idx1))
                        {
                            flow_direction = flow_direction_data(idx0, idx1);

                            if (flow_direction == north_west<FlowDirectionElement>)
                            {
                                output_cells_idxs[accu::Direction::west].push_back({idx0, idx1});
                                input_cell_idxs[accu::Direction::west].push_back(idx0 - 1);
                            }
                            else if (flow_direction == west<FlowDirectionElement>)
                            {
                                output_cells_idxs[accu::Direction::west].push_back({idx0, idx1});
                                input_cell_idxs[accu::Direction::west].push_back(idx0);
                            }
                            else if (flow_direction == south_west<FlowDirectionElement>)
                            {
                                output_cells_idxs[accu::Direction::west].push_back({idx0, idx1});
                                input_cell_idxs[accu::Direction::west].push_back(idx0 + 1);
                            }
                        }
                    }

                    // East side, excluding north-east and south-east corners
                    idx1 = extent1 - 1;

                    for (idx0 = 1; idx0 < extent0 - 1; ++idx0)
                    {
                        if (!indp.is_no_data(flow_direction_data, idx0, idx1))
                        {
                            flow_direction = flow_direction_data(idx0, idx1);

                            if (flow_direction == north_east<FlowDirectionElement>)
                            {
                                output_cells_idxs[accu::Direction::east].push_back({idx0, idx1});
                                input_cell_idxs[accu::Direction::east].push_back(idx0 - 1);
                            }
                            else if (flow_direction == east<FlowDirectionElement>)
                            {
                                output_cells_idxs[accu::Direction::east].push_back({idx0, idx1});
                                input_cell_idxs[accu::Direction::east].push_back(idx0);
                            }
                            else if (flow_direction == south_east<FlowDirectionElement>)
                            {
                                output_cells_idxs[accu::Direction::east].push_back({idx0, idx1});
                                input_cell_idxs[accu::Direction::east].push_back(idx0 + 1);
                            }
                        }
                    }
                }

                // North-west corner
                {
                    idx0 = 0;
                    idx1 = 0;

                    if (!indp.is_no_data(flow_direction_data, idx0, idx1))
                    {
                        flow_direction = flow_direction_data(idx0, idx1);

                        if (flow_direction == south_west<FlowDirectionElement>)
                        {
                            output_cells_idxs[accu::Direction::west].push_back({idx0, idx1});
                            input_cell_idxs[accu::Direction::west].push_back(idx0 + 1);
                        }
                        else if (flow_direction == west<FlowDirectionElement>)
                        {
                            output_cells_idxs[accu::Direction::west].push_back({idx0, idx1});
                            input_cell_idxs[accu::Direction::west].push_back(idx0);
                        }
                        else if (flow_direction == north_west<FlowDirectionElement>)
                        {
                            output_cells_idxs[accu::Direction::north_west].push_back({idx0, idx1});
                            input_cell_idxs[accu::Direction::north_west].push_back(std::size_t(-1));
                        }
                        else if (flow_direction == north<FlowDirectionElement>)
                        {
                            output_cells_idxs[accu::Direction::north].push_back({idx0, idx1});
                            input_cell_idxs[accu::Direction::north].push_back(idx1);
                        }
                        else if (flow_direction == north_east<FlowDirectionElement>)
                        {
                            output_cells_idxs[accu::Direction::north].push_back({idx0, idx1});
                            input_cell_idxs[accu::Direction::north].push_back(idx1 + 1);
                        }
                    }
                }

                // North-east corner
                {
                    idx0 = 0;
                    idx1 = extent1 - 1;

                    if (!indp.is_no_data(flow_direction_data, idx0, idx1))
                    {
                        flow_direction = flow_direction_data(idx0, idx1);

                        if (flow_direction == north_west<FlowDirectionElement>)
                        {
                            output_cells_idxs[accu::Direction::north].push_back({idx0, idx1});
                            input_cell_idxs[accu::Direction::north].push_back(idx1 - 1);
                        }
                        else if (flow_direction == north<FlowDirectionElement>)
                        {
                            output_cells_idxs[accu::Direction::north].push_back({idx0, idx1});
                            input_cell_idxs[accu::Direction::north].push_back(idx1);
                        }
                        else if (flow_direction == north_east<FlowDirectionElement>)
                        {
                            output_cells_idxs[accu::Direction::north_east].push_back({idx0, idx1});
                            input_cell_idxs[accu::Direction::north_east].push_back(std::size_t(-1));
                        }
                        else if (flow_direction == east<FlowDirectionElement>)
                        {
                            output_cells_idxs[accu::Direction::east].push_back({idx0, idx1});
                            input_cell_idxs[accu::Direction::east].push_back(idx0);
                        }
                        else if (flow_direction == south_east<FlowDirectionElement>)
                        {
                            output_cells_idxs[accu::Direction::east].push_back({idx0, idx1});
                            input_cell_idxs[accu::Direction::east].push_back(idx0 + 1);
                        }
                    }
                }

                // South-east corner
                {
                    idx0 = extent0 - 1;
                    idx1 = extent1 - 1;

                    if (!indp.is_no_data(flow_direction_data, idx0, idx1))
                    {
                        flow_direction = flow_direction_data(idx0, idx1);

                        if (flow_direction == north_east<FlowDirectionElement>)
                        {
                            output_cells_idxs[accu::Direction::east].push_back({idx0, idx1});
                            input_cell_idxs[accu::Direction::east].push_back(idx0 - 1);
                        }
                        else if (flow_direction == east<FlowDirectionElement>)
                        {
                            output_cells_idxs[accu::Direction::east].push_back({idx0, idx1});
                            input_cell_idxs[accu::Direction::east].push_back(idx0);
                        }
                        else if (flow_direction == south_east<FlowDirectionElement>)
                        {
                            output_cells_idxs[accu::Direction::south_east].push_back({idx0, idx1});
                            input_cell_idxs[accu::Direction::south_east].push_back(std::size_t(-1));
                        }
                        else if (flow_direction == south<FlowDirectionElement>)
                        {
                            output_cells_idxs[accu::Direction::south].push_back({idx0, idx1});
                            input_cell_idxs[accu::Direction::south].push_back(idx1);
                        }
                        else if (flow_direction == south_west<FlowDirectionElement>)
                        {
                            output_cells_idxs[accu::Direction::south].push_back({idx0, idx1});
                            input_cell_idxs[accu::Direction::south].push_back(idx1 - 1);
                        }
                    }
                }

                // South-west corner
                {
                    idx0 = extent0 - 1;
                    idx1 = 0;

                    if (!indp.is_no_data(flow_direction_data, idx0, idx1))
                    {
                        flow_direction = flow_direction_data(idx0, idx1);

                        if (flow_direction == north_west<FlowDirectionElement>)
                        {
                            output_cells_idxs[accu::Direction::west].push_back({idx0, idx1});
                            input_cell_idxs[accu::Direction::west].push_back(idx0 - 1);
                        }
                        else if (flow_direction == west<FlowDirectionElement>)
                        {
                            output_cells_idxs[accu::Direction::west].push_back({idx0, idx1});
                            input_cell_idxs[accu::Direction::west].push_back(idx0);
                        }
                        else if (flow_direction == south_west<FlowDirectionElement>)
                        {
                            output_cells_idxs[accu::Direction::south_west].push_back({idx0, idx1});
                            input_cell_idxs[accu::Direction::south_west].push_back(std::size_t(-1));
                        }
                        else if (flow_direction == south<FlowDirectionElement>)
                        {
                            output_cells_idxs[accu::Direction::south].push_back({idx0, idx1});
                            input_cell_idxs[accu::Direction::south].push_back(idx1);
                        }
                        else if (flow_direction == south_east<FlowDirectionElement>)
                        {
                            output_cells_idxs[accu::Direction::south].push_back({idx0, idx1});
                            input_cell_idxs[accu::Direction::south].push_back(idx1 + 1);
                        }
                    }
                }

                // Each existing task managing a neighbouring partition gets sent a single, possibly empty,
                // collection of cell idxs
                for (accu::Direction const direction : accu::directions)
                {
                    if (communicator.has_neighbour(direction))
                    {
                        communicator.send(direction, std::move(input_cell_idxs[direction]));
                    }
                }
            }

            // For each partition border:
            // - Receive 1D idxs of receiving cells
            hpx::future<std::array<CellsIdxs, nr_neighbours>> input_cells_idxs_f{};

            {
                // For each receive channel, spawn a task that will process all cell indices sent through it,
                // until no values are to be expected anymore. Convert the indices to partition cell indices.
                // Aggregate the collections returned by all tasks.
                std::array<hpx::future<CellsIdxs>, nr_neighbours> received_cells_idxs{};

                received_cells_idxs[accu::Direction::north] = hpx::async(
                    monitor_cell_idx_inputs<std::vector<Index>, RowIdxConverter, rank>,
                    communicator.receive_channel(accu::Direction::north),
                    RowIdxConverter{},
                    flow_direction_data.shape());
                received_cells_idxs[accu::Direction::south] = hpx::async(
                    monitor_cell_idx_inputs<std::vector<Index>, RowIdxConverter, rank>,
                    communicator.receive_channel(accu::Direction::south),
                    RowIdxConverter{extent0 - 1},
                    flow_direction_data.shape());

                received_cells_idxs[accu::Direction::west] = hpx::async(
                    monitor_cell_idx_inputs<std::vector<Index>, ColIdxConverter, rank>,
                    communicator.receive_channel(accu::Direction::west),
                    ColIdxConverter{},
                    flow_direction_data.shape());
                received_cells_idxs[accu::Direction::east] = hpx::async(
                    monitor_cell_idx_inputs<std::vector<Index>, ColIdxConverter, rank>,
                    communicator.receive_channel(accu::Direction::east),
                    ColIdxConverter{extent1 - 1},
                    flow_direction_data.shape());

                received_cells_idxs[accu::Direction::north_west] = hpx::async(
                    monitor_cell_idx_inputs<std::vector<Index>, CornerIdxConverter, rank>,
                    communicator.receive_channel(accu::Direction::north_west),
                    CornerIdxConverter{},
                    flow_direction_data.shape());
                received_cells_idxs[accu::Direction::north_east] = hpx::async(
                    monitor_cell_idx_inputs<std::vector<Index>, CornerIdxConverter, rank>,
                    communicator.receive_channel(accu::Direction::north_east),
                    CornerIdxConverter{0, extent1 - 1},
                    flow_direction_data.shape());
                received_cells_idxs[accu::Direction::south_east] = hpx::async(
                    monitor_cell_idx_inputs<std::vector<Index>, CornerIdxConverter, rank>,
                    communicator.receive_channel(accu::Direction::south_east),
                    CornerIdxConverter{extent0 - 1, extent1 - 1},
                    flow_direction_data.shape());
                received_cells_idxs[accu::Direction::south_west] = hpx::async(
                    monitor_cell_idx_inputs<std::vector<Index>, CornerIdxConverter, rank>,
                    communicator.receive_channel(accu::Direction::south_west),
                    CornerIdxConverter{extent0 - 1, 0},
                    flow_direction_data.shape());

                input_cells_idxs_f =
                    hpx::when_all(std::move(received_cells_idxs))
                        .then(
                            hpx::unwrapping(

                                [](std::array<hpx::future<CellsIdxs>, nr_neighbours> idxs_fs) -> auto
                                {
                                    std::array<CellsIdxs, detail::nr_neighbours<rank>()> cells_idxs{};

                                    std::transform(
                                        idxs_fs.begin(),
                                        idxs_fs.end(),
                                        cells_idxs.begin(),
                                        [](auto& idxs_f) -> auto { return idxs_f.get(); });

                                    return cells_idxs;
                                }

                                ));
            }

            return hpx::make_tuple(std::move(input_cells_idxs_f), std::move(output_cells_idxs));
        }


        template<typename Policies, typename FlowDirectionElement, Rank rank>
        auto connectivity(
            Policies const& policies,
            ArrayPartition<FlowDirectionElement, rank> const& flow_direction_partition,
            InflowCountCommunicator<rank>&& communicator)
            -> hpx::tuple<
                hpx::future<std::array<std::vector<std::array<Index, rank>>, nr_neighbours<rank>()>>,
                hpx::future<std::array<std::vector<std::array<Index, rank>>, nr_neighbours<rank>()>>>
        {
            using FlowDirectionPartition = ArrayPartition<FlowDirectionElement, rank>;

            return hpx::split_future(
                hpx::dataflow(
                    hpx::launch::async,

                    [policies, communicator = std::move(communicator)](
                        FlowDirectionPartition const& flow_direction_partition) mutable -> auto
                    {
                        AnnotateFunction annotation{"connectivity"};

                        return connectivity_ready(
                            policies, flow_direction_partition, std::move(communicator));
                    },

                    flow_direction_partition));
        }


        template<typename CountElement, typename Policies, typename FlowDirectionElement, Rank rank>
        auto inflow_count_ready(
            Policies const& policies,
            ArrayPartition<FlowDirectionElement, rank> const& flow_direction_partition,
            std::array<std::vector<std::array<Index, rank>>, nr_neighbours<rank>()> const& input_cells_idxs)
            -> ArrayPartition<CountElement, rank>
        {
            lue_hpx_assert(flow_direction_partition.is_ready());

            auto const partition_offset = flow_direction_partition.offset(hpx::launch::sync);
            auto const flow_direction_data = flow_direction_partition.data(hpx::launch::sync);

            using CountPartition = ArrayPartition<CountElement, rank>;
            using CountData = DataT<CountPartition>;

            CountData inflow_count_data{
                inflow_count_partition_data<CountElement>(policies, flow_direction_data)};

            // Finish by updating the counts of those cells at the border of the partition that receive
            // inputs from neighbouring partitions
            auto const& partition_shape{flow_direction_data.shape()};
            [[maybe_unused]] auto const [extent0, extent1] = partition_shape;
            [[maybe_unused]] auto const& indp{std::get<0>(policies.inputs_policies()).input_no_data_policy()};
            [[maybe_unused]] auto const& ondp{
                std::get<0>(policies.outputs_policies()).output_no_data_policy()};

            for (Index direction_idx = 0; direction_idx < nr_neighbours<rank>(); ++direction_idx)
            {
                for (auto const& input_cell_idxs : input_cells_idxs[direction_idx])
                {
                    auto [idx0, idx1] = input_cell_idxs;

                    lue_hpx_assert(idx0 >= 0 && idx0 < extent0);
                    lue_hpx_assert(idx1 >= 0 && idx1 < extent1);
                    lue_hpx_assert((idx0 == 0 || idx0 == extent0 - 1) || (idx1 == 0 || idx1 == extent1 - 1));
                    lue_hpx_assert(!indp.is_no_data(flow_direction_data, idx0, idx1));
                    lue_hpx_assert(!ondp.is_no_data(inflow_count_data, idx0, idx1));

                    inflow_count_data(idx0, idx1) += 1;

                    lue_hpx_assert(inflow_count_data(idx0, idx1) <= 8);
                }
            }

            return CountPartition{hpx::find_here(), partition_offset, std::move(inflow_count_data)};
        }


        template<typename CountElement, typename Policies, typename FlowDirectionElement, Rank rank>
        auto inflow_count_action(
            Policies const& policies,
            ArrayPartition<FlowDirectionElement, rank> const& flow_direction_partition,
            InflowCountCommunicator<rank> inflow_count_communicator)
            -> hpx::tuple<
                ArrayPartition<CountElement, rank>,
                hpx::shared_future<std::array<std::vector<std::array<Index, rank>>, nr_neighbours<rank>()>>,
                hpx::future<std::array<std::vector<std::array<Index, rank>>, nr_neighbours<rank>()>>>
        {
            AnnotateFunction annotation{"inflow_count"};

            // Determine connectivity between this partition and the neighbouring partitions
            using FlowDirectionPartition = ArrayPartition<FlowDirectionElement, rank>;
            using CellsIdxs = std::vector<std::array<Index, rank>>;

            hpx::shared_future<std::array<CellsIdxs, nr_neighbours<rank>()>> input_cells_idxs_f{};
            hpx::future<std::array<CellsIdxs, nr_neighbours<rank>()>> output_cells_idxs_f{};

            hpx::tie(input_cells_idxs_f, output_cells_idxs_f) =
                connectivity(policies, flow_direction_partition, std::move(inflow_count_communicator));

            // Calculate inflow count of each cell
            using CountPartition = ArrayPartition<CountElement, rank>;

            // Once both the flow direction partition and the input cells idxs are available, forward to the
            // function that determines the inflow count. That function does two things:
            // - Determine inflow counts based on flow directions, and
            // - Merge the information about input cells
            // This could be split into to tasks, but experiments show that the result scales worse.
            // Apparently, the resulting tasks become too small. Guideline: sometimes it is better to wait on
            // more information in order to be able to do more within a single task.
            CountPartition count_partition = hpx::dataflow(
                hpx::launch::async,

                [policies](
                    FlowDirectionPartition const& flow_direction_partition,
                    hpx::shared_future<std::array<CellsIdxs, nr_neighbours<rank>()>> const&
                        input_cells_idxs_f) -> ArrayPartition<CountElement, rank>
                {
                    return inflow_count_ready<CountElement>(
                        policies, flow_direction_partition, input_cells_idxs_f.get());
                },

                flow_direction_partition,
                input_cells_idxs_f);

            return hpx::make_tuple(
                std::move(count_partition), std::move(input_cells_idxs_f), std::move(output_cells_idxs_f));
        }


        template<typename CountElement, typename Policies, typename FlowDirectionElement, Rank rank>
        struct InflowCountAction:
            hpx::actions::make_action<
                decltype(&inflow_count_action<CountElement, Policies, FlowDirectionElement, rank>),
                &inflow_count_action<CountElement, Policies, FlowDirectionElement, rank>,
                InflowCountAction<CountElement, Policies, FlowDirectionElement, rank>>::type
        {
        };


        template<Rank rank>
        auto inflow_count_communicators_use_count_by() -> root::ResourceUseCountByKey<LocalitiesPtr<rank>>&
        {
            static root::ResourceUseCountByKey<LocalitiesPtr<rank>> use_count_by{};

            return use_count_by;
        }


        template<Rank rank>
        auto inflow_count_communicators_use_finished() -> root::ResourceUseFinished<LocalitiesPtr<rank>>&
        {
            static root::ResourceUseFinished<LocalitiesPtr<rank>> use_finished{};

            return use_finished;
        }

    }  // namespace detail


    template<typename CountElement, typename Policies, typename FlowDirectionElement, Rank rank>
    auto inflow_count(
        Policies const& policies, PartitionedArray<FlowDirectionElement, rank> const& flow_direction)
        -> PartitionedArray<CountElement, rank>
    {
        // If this is the first time we are called, for this specific distribution of partitions, then create
        // a new set of channels to use. Otherwise, reuse the existing ones.

        using InflowCountArray = PartitionedArray<CountElement, rank>;
        Localities<rank> const& localities{flow_direction.localities()};
        LocalitiesPtr<rank> localities_ptr{flow_direction.localities_ptr()};

        using InflowCountCommunicatorArray =
            detail::CommunicatorArray<detail::InflowCountCommunicator<rank>, rank>;

        // A map for caching the communicator array by a certain distribution of partitions over localities.
        // Per distribution, we can re-use the same communicators. This saves time and memory.
        // NOTE: We could use an ordered map and a max size to prevent the map to increase in size (this
        // only happens if many different partition distributions are passed in, which is likely unlikely
        // in real-world cases).
        static std::map<LocalitiesPtr<rank>, std::unique_ptr<InflowCountCommunicatorArray>>
            communicators_by_localities{};

        if (!communicators_by_localities.contains(localities_ptr))
        {
            communicators_by_localities[localities_ptr] = std::make_unique<InflowCountCommunicatorArray>(
                std::format("/lue/inflow_count/{}", communicators_by_localities.size()), localities);
        }

        lue_hpx_assert(communicators_by_localities.contains(localities_ptr));

        InflowCountCommunicatorArray const& inflow_count_communicators =
            *(communicators_by_localities.find(localities_ptr)->second);

        // We have a set of channels to use. Serialize code that makes use of these channels. Don't let
        // subsequent tasks overtake each other.

        // A count representing this call. The first time this function is called, the count is 1, etc.
        Count const communicators_use_count = detail::root::resource_use_count_by(
            detail::inflow_count_communicators_use_count_by<rank>(), localities_ptr);

        // A future which becomes ready once the previous call to this function, with a count 1 less than the
        // current one, is done using the channels
        hpx::shared_future<void> precondition_f = detail::root::resource_use_finished(
            detail::inflow_count_communicators_use_finished<rank>(),
            localities_ptr,
            communicators_use_count - 1);

        // Continue once the precondition is met. This serializes multiple calls to this function... This is
        // unlikely a problem in real-world situations where there's always more to do.
        using InflowCountPartition = PartitionT<InflowCountArray>;
        using InflowCountPartitions = PartitionsT<InflowCountArray>;

        hpx::future<std::vector<InflowCountPartition>> inflow_count_partitions_f = precondition_f.then(
            [policies,
             flow_direction_partitions = flow_direction.partitions(),
             localities,
             inflow_count_communicators](
                [[maybe_unused]] hpx::shared_future<void> const& communicators_ready_for_us)
                -> std::vector<InflowCountPartition>
            {
                // This code only runs when the communicators are ready for us. Do whatever it takes to
                // compute a result and return the resulting partitions.

                Count const nr_partitions{localities.nr_elements()};

                // For each partition, spawn a task that will solve the calculation for the partition
                std::vector<InflowCountPartition> inflow_count_partitions(nr_partitions);

                detail::InflowCountAction<CountElement, Policies, FlowDirectionElement, rank> action{};

                for (Index partition_idx = 0; partition_idx < nr_partitions; ++partition_idx)
                {
                    inflow_count_partitions[partition_idx] = hpx::get<0>(hpx::split_future(
                        hpx::async(
                            hpx::annotated_function(action, "inflow_count"),
                            localities[partition_idx],
                            policies,
                            flow_direction_partitions[partition_idx],
                            std::move(inflow_count_communicators[partition_idx]))));
                }

                return inflow_count_partitions;
            });

        InflowCountPartitions inflow_count_partitions(flow_direction.partitions().shape());

        {
            Count const nr_partitions{localities.nr_elements()};

            // future<vector<partition>> -> vector<future<partition>>
            std::vector<hpx::future<InflowCountPartition>> inflow_count_partition_fs =
                hpx::split_future<InflowCountPartition>(std::move(inflow_count_partitions_f), nr_partitions);

            // vector<future<partition>> -> partitions
            for (Index partition_idx = 0; partition_idx < nr_partitions; ++partition_idx)
            {
                lue_hpx_assert(!inflow_count_partitions[partition_idx].valid());
                lue_hpx_assert(inflow_count_partition_fs[partition_idx].valid());

                inflow_count_partitions[partition_idx] = inflow_count_partition_fs[partition_idx].then(
                    [](auto&& partition_f) -> InflowCountPartition
                    {
                        lue_hpx_assert(partition_f.valid());
                        lue_hpx_assert(partition_f.is_ready());
                        return InflowCountPartition{partition_f.get()};
                    });

                lue_hpx_assert(inflow_count_partitions[partition_idx].valid());
            }
        }

        hpx::future<void> finished_f =
            hpx::when_all(inflow_count_partitions.begin(), inflow_count_partitions.end());

        detail::root::add_resource_use_finished(
            detail::inflow_count_communicators_use_finished<rank>(),
            localities_ptr,
            communicators_use_count,
            std::move(finished_f));

        detail::root::resource_use_finished(
            detail::inflow_count_communicators_use_finished<rank>(), localities_ptr, communicators_use_count)
            .then(
                [localities_ptr,
                 communicators_use_count]([[maybe_unused]] hpx::shared_future<void> const& finished_f) -> auto
                {
                    detail::root::resource_use_handled(
                        detail::inflow_count_communicators_use_finished<rank>(),
                        localities_ptr,
                        communicators_use_count - 1);
                });

        return {flow_direction, std::move(inflow_count_partitions)};
    }

}  // namespace lue


#define LUE_INSTANTIATE_INFLOW_COUNT(Policies, CountElement, FlowDirectionElement)                           \
                                                                                                             \
    template LUE_ROUTING_OPERATION_EXPORT PartitionedArray<CountElement, 2>                                  \
    inflow_count<CountElement, ArgumentType<void(Policies)>, FlowDirectionElement, 2>(                       \
        ArgumentType<void(Policies)> const&, PartitionedArray<FlowDirectionElement, 2> const&);
