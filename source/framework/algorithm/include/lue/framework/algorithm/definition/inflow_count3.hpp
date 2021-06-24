#pragma once
#include "lue/framework/algorithm/inflow_count.hpp"  // inflow_count_partition_data
#include "lue/framework/algorithm/inflow_count3.hpp"
#include "lue/framework/algorithm/detail/communicator_array.hpp"
#include "lue/framework/algorithm/detail/inflow_count3.hpp"
#include "lue/framework/algorithm/export.hpp"
#include "lue/macro.hpp"


namespace lue {
    namespace detail {

        template<
            typename T,
            typename IdxConverter,
            Rank rank>
        std::vector<std::array<Index, rank>> monitor_channel_inputs(
            hpx::lcos::channel<T>&& channel,
            IdxConverter&& idx_to_idxs)
        {
            std::vector<std::array<Index, rank>> cells_idxs{};

            if(channel)
            {
                for(auto const& idxs: channel)
                {
                    if(idxs.empty())
                    {
                        break;
                    }

                    // For each idx in the collection, create a set of
                    // idxs, based on the shape of the partition
                    cells_idxs.reserve(cells_idxs.size() + idxs.size());
                    std::transform(idxs.begin(), idxs.end(), std::back_inserter(cells_idxs), idx_to_idxs);
                }
            }

            return cells_idxs;
        }


        template<
            typename Policies,
            typename FlowDirectionElement,
            Rank rank>
        hpx::tuple<
            hpx::future<std::array<std::vector<std::array<Index, rank>>, nr_neighbours<rank>()>>,
            std::vector<std::array<Index, rank>>>
                connectivity_ready(
                    Policies const& policies,
                    ArrayPartition<FlowDirectionElement, rank> const& flow_direction_partition,
                    InflowCountCommunicator<rank>&& communicator)
        {
            lue_hpx_assert(flow_direction_partition.is_ready());

            using FlowDirectionPartition = ArrayPartition<FlowDirectionElement, rank>;
            using FlowDirectionData = DataT<FlowDirectionPartition>;
            using CellsIdxs = std::vector<std::array<Index, rank>>;

            auto const flow_direction_partition_ptr{ready_component_ptr(flow_direction_partition)};
            FlowDirectionData const flow_direction_data{flow_direction_partition_ptr->data()};
            auto const [extent0, extent1] = flow_direction_data.shape();

            // - For each partition border
            //     - For each cell that flows into a neighbouring partition
            //         - Determine the neighbouring partition (n, ne, e, ...)
            //         - Determine the cell idx in the neighbouring partition
            //         - Store relation in collection, ordered by
            //           neighbouring partition
            //     - Send idxs of receiving cells
            CellsIdxs output_cells_idxs{};
            {
                auto const& indp{std::get<0>(policies.inputs_policies()).input_no_data_policy()};
                Index idx0, idx1;
                FlowDirectionElement flow_direction;
                std::vector<Index> input_cell_idxs;

                if(extent1 > 2)
                {
                    // North side, excluding north-west and north-east corners
                    input_cell_idxs.clear();
                    idx0 = 0;

                    for(idx1 = 1; idx1 < extent1 - 1; ++idx1)
                    {
                        if(!indp.is_no_data(flow_direction_data, idx0, idx1))
                        {
                            flow_direction = flow_direction_data(idx0, idx1);

                            if(flow_direction == north_west<FlowDirectionElement>)
                            {
                                output_cells_idxs.push_back({idx0, idx1});
                                input_cell_idxs.push_back(idx1 - 1);
                            }
                            else if(flow_direction == north<FlowDirectionElement>)
                            {
                                output_cells_idxs.push_back({idx0, idx1});
                                input_cell_idxs.push_back(idx1);
                            }
                            else if(flow_direction == north_east<FlowDirectionElement>)
                            {
                                output_cells_idxs.push_back({idx0, idx1});
                                input_cell_idxs.push_back(idx1 + 1);
                            }
                        }
                    }

                    if(communicator.has_neighbour(accu::Direction::north))
                    {
                        communicator.send(accu::Direction::north, input_cell_idxs);
                    }

                    // South side, excluding south-west and south-east corners
                    input_cell_idxs.clear();
                    idx0 = extent0 - 1;

                    for(idx1 = 1; idx1 < extent1 - 1; ++idx1)
                    {
                        if(!indp.is_no_data(flow_direction_data, idx0, idx1))
                        {
                            flow_direction = flow_direction_data(idx0, idx1);

                            if(flow_direction == south_west<FlowDirectionElement>)
                            {
                                output_cells_idxs.push_back({idx0, idx1});
                                input_cell_idxs.push_back(idx1 - 1);
                            }
                            else if(flow_direction == south<FlowDirectionElement>)
                            {
                                output_cells_idxs.push_back({idx0, idx1});
                                input_cell_idxs.push_back(idx1);
                            }
                            else if(flow_direction == south_east<FlowDirectionElement>)
                            {
                                output_cells_idxs.push_back({idx0, idx1});
                                input_cell_idxs.push_back(idx1 + 1);
                            }
                        }

                    }

                    if(communicator.has_neighbour(accu::Direction::south))
                    {
                        communicator.send(accu::Direction::south, input_cell_idxs);
                    }
                }

                if(extent0 > 2)
                {
                    // West side, excluding north-west and south-west corners
                    input_cell_idxs.clear();
                    idx1 = 0;

                    for(idx0 = 1; idx0 < extent0 - 1; ++idx0)
                    {
                        if(!indp.is_no_data(flow_direction_data, idx0, idx1))
                        {
                            flow_direction = flow_direction_data(idx0, idx1);

                            if(flow_direction == north_west<FlowDirectionElement>)
                            {
                                output_cells_idxs.push_back({idx0, idx1});
                                input_cell_idxs.push_back(idx0 - 1);
                            }
                            else if(flow_direction == west<FlowDirectionElement>)
                            {
                                output_cells_idxs.push_back({idx0, idx1});
                                input_cell_idxs.push_back(idx0);
                            }
                            else if(flow_direction == south_west<FlowDirectionElement>)
                            {
                                output_cells_idxs.push_back({idx0, idx1});
                                input_cell_idxs.push_back(idx0 + 1);
                            }
                        }
                    }

                    if(communicator.has_neighbour(accu::Direction::west))
                    {
                        communicator.send(accu::Direction::west, input_cell_idxs);
                    }

                    // East side, excluding north-east and south-east corners
                    input_cell_idxs.clear();
                    idx1 = extent1 - 1;

                    for(idx0 = 1; idx0 < extent0 - 1; ++idx0)
                    {
                        if(!indp.is_no_data(flow_direction_data, idx0, idx1))
                        {
                            flow_direction = flow_direction_data(idx0, idx1);

                            if(flow_direction == north_east<FlowDirectionElement>)
                            {
                                output_cells_idxs.push_back({idx0, idx1});
                                input_cell_idxs.push_back(idx0 - 1);
                            }
                            else if(flow_direction == east<FlowDirectionElement>)
                            {
                                output_cells_idxs.push_back({idx0, idx1});
                                input_cell_idxs.push_back(idx0);
                            }
                            else if(flow_direction == south_east<FlowDirectionElement>)
                            {
                                output_cells_idxs.push_back({idx0, idx1});
                                input_cell_idxs.push_back(idx0 + 1);
                            }
                        }
                    }

                    if(communicator.has_neighbour(accu::Direction::east))
                    {
                        communicator.send(accu::Direction::east, input_cell_idxs);
                    }
                }

                // North-west corner
                {
                    input_cell_idxs.clear();
                    idx0 = 0;
                    idx1 = 0;

                    if(!indp.is_no_data(flow_direction_data, idx0, idx1))
                    {
                        flow_direction = flow_direction_data(idx0, idx1);

                        if(flow_direction == south_west<FlowDirectionElement>)
                        {
                            output_cells_idxs.push_back({idx0, idx1});

                            if(communicator.has_neighbour(accu::Direction::west))
                            {
                                input_cell_idxs.push_back(idx0 + 1);
                                communicator.send(accu::Direction::west, input_cell_idxs);
                            }
                        }
                        else if(flow_direction == west<FlowDirectionElement>)
                        {
                            output_cells_idxs.push_back({idx0, idx1});

                            if(communicator.has_neighbour(accu::Direction::west))
                            {
                                input_cell_idxs.push_back(idx0);
                                communicator.send(accu::Direction::west, input_cell_idxs);
                            }
                        }
                        else if(flow_direction == north_west<FlowDirectionElement>)
                        {
                            output_cells_idxs.push_back({idx0, idx1});

                            if(communicator.has_neighbour(accu::Direction::north_west))
                            {
                                input_cell_idxs.push_back(std::size_t(-1));
                                communicator.send(accu::Direction::north_west, input_cell_idxs);
                            }
                        }
                        else if(flow_direction == north<FlowDirectionElement>)
                        {
                            output_cells_idxs.push_back({idx0, idx1});

                            if(communicator.has_neighbour(accu::Direction::north))
                            {
                                input_cell_idxs.push_back(idx1);
                                communicator.send(accu::Direction::north, input_cell_idxs);
                            }
                        }
                        else if(flow_direction == north_east<FlowDirectionElement>)
                        {
                            output_cells_idxs.push_back({idx0, idx1});

                            if(communicator.has_neighbour(accu::Direction::north))
                            {
                                input_cell_idxs.push_back(idx1 + 1);
                                communicator.send(accu::Direction::north, input_cell_idxs);
                            }
                        }
                    }
                }

                // North-east corner
                {
                    input_cell_idxs.clear();
                    idx0 = 0;
                    idx1 = extent1 - 1;

                    if(!indp.is_no_data(flow_direction_data, idx0, idx1))
                    {
                        flow_direction = flow_direction_data(idx0, idx1);

                        if(flow_direction == north_west<FlowDirectionElement>)
                        {
                            output_cells_idxs.push_back({idx0, idx1});

                            if(communicator.has_neighbour(accu::Direction::north))
                            {
                                input_cell_idxs.push_back(idx1 - 1);
                                communicator.send(accu::Direction::north, input_cell_idxs);
                            }
                        }
                        else if(flow_direction == north<FlowDirectionElement>)
                        {
                            output_cells_idxs.push_back({idx0, idx1});

                            if(communicator.has_neighbour(accu::Direction::north))
                            {
                                input_cell_idxs.push_back(idx1);
                                communicator.send(accu::Direction::north, input_cell_idxs);
                            }
                        }
                        else if(flow_direction == north_east<FlowDirectionElement>)
                        {
                            output_cells_idxs.push_back({idx0, idx1});

                            if(communicator.has_neighbour(accu::Direction::north_east))
                            {
                                input_cell_idxs.push_back(std::size_t(-1));
                                communicator.send(accu::Direction::north_east, input_cell_idxs);
                            }
                        }
                        else if(flow_direction == east<FlowDirectionElement>)
                        {
                            output_cells_idxs.push_back({idx0, idx1});

                            if(communicator.has_neighbour(accu::Direction::east))
                            {
                                input_cell_idxs.push_back(idx0);
                                communicator.send(accu::Direction::east, input_cell_idxs);
                            }
                        }
                        else if(flow_direction == south_east<FlowDirectionElement>)
                        {
                            output_cells_idxs.push_back({idx0, idx1});

                            if(communicator.has_neighbour(accu::Direction::east))
                            {
                                input_cell_idxs.push_back(idx0 + 1);
                                communicator.send(accu::Direction::east, input_cell_idxs);
                            }
                        }
                    }
                }

                // South-east corner
                {
                    input_cell_idxs.clear();
                    idx0 = extent1 - 1;
                    idx1 = extent1 - 1;

                    if(!indp.is_no_data(flow_direction_data, idx0, idx1))
                    {
                        flow_direction = flow_direction_data(idx0, idx1);

                        if(flow_direction == north_east<FlowDirectionElement>)
                        {
                            output_cells_idxs.push_back({idx0, idx1});

                            if(communicator.has_neighbour(accu::Direction::east))
                            {
                                input_cell_idxs.push_back(idx0 - 1);
                                communicator.send(accu::Direction::east, input_cell_idxs);
                            }
                        }
                        else if(flow_direction == east<FlowDirectionElement>)
                        {
                            output_cells_idxs.push_back({idx0, idx1});

                            if(communicator.has_neighbour(accu::Direction::east))
                            {
                                input_cell_idxs.push_back(idx0);
                                communicator.send(accu::Direction::east, input_cell_idxs);
                            }
                        }
                        else if(flow_direction == south_east<FlowDirectionElement>)
                        {
                            output_cells_idxs.push_back({idx0, idx1});

                            if(communicator.has_neighbour(accu::Direction::south_east))
                            {
                                input_cell_idxs.push_back(std::size_t(-1));
                                communicator.send(accu::Direction::south_east, input_cell_idxs);
                            }
                        }
                        else if(flow_direction == south<FlowDirectionElement>)
                        {
                            output_cells_idxs.push_back({idx0, idx1});

                            if(communicator.has_neighbour(accu::Direction::south))
                            {
                                input_cell_idxs.push_back(idx1);
                                communicator.send(accu::Direction::south, input_cell_idxs);
                            }
                        }
                        else if(flow_direction == south_west<FlowDirectionElement>)
                        {
                            output_cells_idxs.push_back({idx0, idx1});

                            if(communicator.has_neighbour(accu::Direction::south))
                            {
                                input_cell_idxs.push_back(idx1 - 1);
                                communicator.send(accu::Direction::south, input_cell_idxs);
                            }
                        }
                    }
                }

                // South-west corner
                {
                    input_cell_idxs.clear();
                    idx0 = extent1 - 1;
                    idx1 = 0;

                    if(!indp.is_no_data(flow_direction_data, idx0, idx1))
                    {
                        flow_direction = flow_direction_data(idx0, idx1);

                        if(flow_direction == north_west<FlowDirectionElement>)
                        {
                            output_cells_idxs.push_back({idx0, idx1});

                            if(communicator.has_neighbour(accu::Direction::west))
                            {
                                input_cell_idxs.push_back(idx0 - 1);
                                communicator.send(accu::Direction::west, input_cell_idxs);
                            }
                        }
                        else if(flow_direction == west<FlowDirectionElement>)
                        {
                            output_cells_idxs.push_back({idx0, idx1});

                            if(communicator.has_neighbour(accu::Direction::west))
                            {
                                input_cell_idxs.push_back(idx0);
                                communicator.send(accu::Direction::west, input_cell_idxs);
                            }
                        }
                        else if(flow_direction == south_west<FlowDirectionElement>)
                        {
                            output_cells_idxs.push_back({idx0, idx1});

                            if(communicator.has_neighbour(accu::Direction::south_west))
                            {
                                input_cell_idxs.push_back(std::size_t(-1));
                                communicator.send(accu::Direction::south_west, input_cell_idxs);
                            }
                        }
                        else if(flow_direction == south<FlowDirectionElement>)
                        {
                            output_cells_idxs.push_back({idx0, idx1});

                            if(communicator.has_neighbour(accu::Direction::south))
                            {
                                input_cell_idxs.push_back(idx1);
                                communicator.send(accu::Direction::south, input_cell_idxs);
                            }
                        }
                        else if(flow_direction == south_east<FlowDirectionElement>)
                        {
                            output_cells_idxs.push_back({idx0, idx1});

                            if(communicator.has_neighbour(accu::Direction::south))
                            {
                                input_cell_idxs.push_back(idx1 + 1);
                                communicator.send(accu::Direction::south, input_cell_idxs);
                            }
                        }
                    }
                }

                // Send empty collection as a sentinel value. The receiver
                // can assume this is the last value sent.
                communicator.send({});
            }

            // - For each partition border
            //     - Receive idxs of receiving cells
            Count const nr_neighbours{detail::nr_neighbours<rank>()};
            hpx::future<std::array<CellsIdxs, nr_neighbours>> input_cells_idxs_f{};
            {
                // For each receive channel, spawn a task that will process all
                // cell indices sent through it, until no values are to
                // be expected anymore. Convert the indices to partition
                // cell indices. Aggregate the collections returned by all tasks.
                std::array<hpx::future<CellsIdxs>, nr_neighbours> received_cells_idxs{};

                // Rules to convert indices received on the various
                // channels, to cells indices:

                // north
                // - row = 0
                // - get col from channel
                // RowIdxConverter{0};

                // south
                // - row = extent0 - 1
                // - get col from channel
                // RowIdxConverter{extent0 - 1};

                // The row is fixed, get the column from the channels
                auto row_idx_converter =
                    [](Index const row, Index const idx)
                    { return std::array<Index, 2>{row, idx}; };
                auto north_idx_converter =
                    [row_idx_converter](Index const idx)
                    { return row_idx_converter(0, idx); };
                auto south_idx_converter =
                    [row_idx_converter, extent0](Index const idx)
                    { return row_idx_converter(extent0 - 1, idx); };

                // west
                // - col = 0
                // - get row from channel
                // ColIdxConverter{0};

                // east
                // - col = extent1 - 1
                // - get row from channel
                // ColIdxConverter{extent1 - 1};

                // The col is fixed, get the row from the channels
                auto col_idx_converter =
                    [](Index const idx, Index const col)
                    { return std::array<Index, 2>{idx, col}; };
                auto west_idx_converter =
                    [col_idx_converter](Index const idx)
                    { return col_idx_converter(idx, 0); };
                auto east_idx_converter =
                    [col_idx_converter, extent1](Index const idx)
                    { return col_idx_converter(idx, extent1 - 1); };

                // north west
                // - row = 0
                // - col = 0
                // - get at most one 'sentinel' idx from channel
                // CornerIdxConverter{0, 0};

                // north east
                // - row = 0
                // - col = extent1 - 1
                // - get at most one 'sentinel' idx from channel
                // CornerIdxConverter{0, extent1 - 1};

                // south east
                // - row = extent0 - 1
                // - col = extent1 - 1
                // - get at most one 'sentinel' idx from channel
                // CornerIdxConverter{extent0 - 1, extent1 - 1};

                // south west
                // - row = extent0 - 1
                // - col = 0
                // - get at most one 'sentinel' idx from channel
                // CornerIdxConverter{extent0 - 1, 0};

                // The row and col are fixed, get whatever value from the channel
                auto corner_idx_converter =
                    [](Index const row, Index const col)
                    { return std::array<Index, 2>{row, col}; };
                auto north_west_idx_converter =
                    [corner_idx_converter]([[maybe_unused]] Index const idx)
                    { return corner_idx_converter(0, 0); };
                auto north_east_idx_converter =
                    [corner_idx_converter, extent1]([[maybe_unused]] Index const idx)
                    { return corner_idx_converter(0, extent1 - 1); };
                auto south_east_idx_converter =
                    [corner_idx_converter, extent0, extent1]([[maybe_unused]] Index const idx)
                    { return corner_idx_converter(extent0 - 1, extent1 - 1); };
                auto south_west_idx_converter =
                    [corner_idx_converter, extent0]([[maybe_unused]] Index const idx)
                    { return corner_idx_converter(extent0 - 1, 0); };


                received_cells_idxs[accu::Direction::north] = hpx::async(
                    monitor_channel_inputs<std::vector<Index>, decltype(north_idx_converter), rank>,
                    communicator.receive_channel(accu::Direction::north), north_idx_converter);
                received_cells_idxs[accu::Direction::south] = hpx::async(
                    monitor_channel_inputs<std::vector<Index>, decltype(south_idx_converter), rank>,
                    communicator.receive_channel(accu::Direction::south), south_idx_converter);

                received_cells_idxs[accu::Direction::west] = hpx::async(
                    monitor_channel_inputs<std::vector<Index>, decltype(west_idx_converter), rank>,
                    communicator.receive_channel(accu::Direction::west), west_idx_converter);
                received_cells_idxs[accu::Direction::east] = hpx::async(
                    monitor_channel_inputs<std::vector<Index>, decltype(east_idx_converter), rank>,
                    communicator.receive_channel(accu::Direction::east), east_idx_converter);

                received_cells_idxs[accu::Direction::north_west] = hpx::async(
                    monitor_channel_inputs<std::vector<Index>, decltype(north_west_idx_converter), rank>,
                    communicator.receive_channel(accu::Direction::north_west), north_west_idx_converter);
                received_cells_idxs[accu::Direction::north_east] = hpx::async(
                    monitor_channel_inputs<std::vector<Index>, decltype(north_east_idx_converter), rank>,
                    communicator.receive_channel(accu::Direction::north_east), north_east_idx_converter);
                received_cells_idxs[accu::Direction::south_east] = hpx::async(
                    monitor_channel_inputs<std::vector<Index>, decltype(south_east_idx_converter), rank>,
                    communicator.receive_channel(accu::Direction::south_east), south_east_idx_converter);
                received_cells_idxs[accu::Direction::south_west] = hpx::async(
                    monitor_channel_inputs<std::vector<Index>, decltype(south_west_idx_converter), rank>,
                    communicator.receive_channel(accu::Direction::south_west), south_west_idx_converter);

                input_cells_idxs_f = hpx::when_all(received_cells_idxs).then(
                        hpx::util::unwrapping(

                                [](std::array<hpx::future<CellsIdxs>, nr_neighbours>&& idxs_fs)
                                {
                                    std::array<CellsIdxs, nr_neighbours> cells_idxs{};

                                    std::transform(idxs_fs.begin(), idxs_fs.end(), cells_idxs.begin(),
                                            [](auto& idxs_f)
                                            {
                                                return idxs_f.get();
                                            }
                                        );

                                    return cells_idxs;
                                }

                            ));
            }

            return hpx::make_tuple(std::move(input_cells_idxs_f), std::move(output_cells_idxs));
        }


        template<
            typename Policies,
            typename FlowDirectionElement,
            Rank rank>
        hpx::tuple<
            hpx::future<std::array<std::vector<std::array<Index, rank>>, nr_neighbours<rank>()>>,
            hpx::future<std::vector<std::array<Index, rank>>>>
                connectivity(
                    Policies const& policies,
                    ArrayPartition<FlowDirectionElement, rank> const& flow_direction_partition,
                    InflowCountCommunicator<rank>&& communicator)
        {
            using FlowDirectionPartition = ArrayPartition<FlowDirectionElement, rank>;
            using CellsIdxs = std::vector<std::array<Index, rank>>;

            hpx::future<std::array<CellsIdxs, nr_neighbours<rank>()>> input_cells_idxs_f{};
            hpx::future<CellsIdxs> output_cells_idxs_f{};

            hpx::tie(input_cells_idxs_f, output_cells_idxs_f) =
                hpx::split_future(hpx::dataflow(
                        hpx::launch::async,

                        [policies, communicator=std::move(communicator)](
                            FlowDirectionPartition const& flow_direction_partition) mutable
                        {
                            return connectivity_ready(
                                policies, flow_direction_partition, std::move(communicator));
                        },

                    flow_direction_partition));

            return hpx::make_tuple(std::move(input_cells_idxs_f), std::move(output_cells_idxs_f));
        }


        template<
            typename CountElement,
            typename Policies,
            typename FlowDirectionElement,
            Rank rank>
        ArrayPartition<CountElement, rank> inflow_count3_ready(
            Policies const& policies,
            ArrayPartition<FlowDirectionElement, rank> const& flow_direction_partition,
            std::array<std::vector<std::array<Index, rank>>, nr_neighbours<rank>()> const& input_cells_idxs)
        {
            lue_hpx_assert(flow_direction_partition.is_ready());

            using FlowDirectionPartition = ArrayPartition<FlowDirectionElement, rank>;
            using FlowDirectionData = DataT<FlowDirectionPartition>;

            auto const flow_direction_partition_ptr{ready_component_ptr(flow_direction_partition)};
            auto const partition_offset{flow_direction_partition_ptr->offset()};
            FlowDirectionData const flow_direction_data{flow_direction_partition_ptr->data()};

            using CountPartition = ArrayPartition<CountElement, rank>;
            using CountData = DataT<CountPartition>;

            CountData inflow_count_data{
                inflow_count_partition_data<CountElement>(policies, flow_direction_data)};

            // Finish by updating the counts of those cells at the border
            // of the partition that receive inputs from neighbouring
            // partitions.
            auto const& partition_shape{flow_direction_data.shape()};
            auto const [extent0, extent1] = partition_shape;

            auto const& indp{std::get<0>(policies.inputs_policies()).input_no_data_policy()};
            auto const& ondp{std::get<0>(policies.outputs_policies()).output_no_data_policy()};

            for(Index d = 0; d < nr_neighbours<rank>(); ++d)
            {
                for(auto const& input_cell_idxs: input_cells_idxs[d])
                {
                    auto [idx0, idx1] = input_cell_idxs;

                    lue_hpx_assert(idx0 >= 0 && idx0 < extent0);
                    lue_hpx_assert(idx1 >= 0 && idx1 < extent1);
                    lue_hpx_assert(
                        (idx0 == 0 || idx0 == extent0 - 1) ||
                        (idx1 == 0 || idx1 == extent1 - 1));
                    lue_hpx_assert(!indp.is_no_data(flow_direction_data, idx0, idx1));
                    lue_hpx_assert(!ondp.is_no_data(inflow_count_data, idx0, idx1));

                    inflow_count_data(idx0, idx1) += 1;

                    lue_hpx_assert(inflow_count_data(idx0, idx1) <= 8);
                }
            }

            return CountPartition{hpx::find_here(), partition_offset, std::move(inflow_count_data)};
        }


        template<
            typename CountElement,
            typename Policies,
            typename FlowDirectionElement,
            Rank rank>
        ArrayPartition<CountElement, rank> inflow_count3(
            Policies const& policies,
            ArrayPartition<FlowDirectionElement, rank> const& flow_direction_partition,
            hpx::shared_future<std::array<std::vector<std::array<Index, rank>>, nr_neighbours<rank>()>>
                input_cells_idxs_f)
        {
            using FlowDirectionPartition = ArrayPartition<FlowDirectionElement, rank>;
            using CellsIdxs = std::vector<std::array<Index, rank>>;

            return hpx::dataflow(
                    hpx::launch::async,

                    [policies](
                        FlowDirectionPartition const& flow_direction_partition,
                        hpx::shared_future<std::array<CellsIdxs, nr_neighbours<rank>()>> const&
                            input_cells_idxs_f)
                    {
                        return inflow_count3_ready<CountElement>(
                            policies, flow_direction_partition, input_cells_idxs_f.get());
                    },

                    flow_direction_partition,
                    input_cells_idxs_f
                );
        }


        template<
            typename CountElement,
            typename Policies,
            typename FlowDirectionElement,
            Rank rank>
        hpx::tuple<
            ArrayPartition<CountElement, rank>,
            hpx::shared_future<std::array<std::vector<std::array<Index, rank>>, nr_neighbours<rank>()>>,
            hpx::future<std::vector<std::array<Index, rank>>>>
                inflow_count3_action(
                    Policies const& policies,
                    ArrayPartition<FlowDirectionElement, rank> const& flow_direction_partition,
                    InflowCountCommunicator<rank> inflow_count_communicator)
        {
            // Determine connectivity between this partition and the neighbouring partitions
            using CellsIdxs = std::vector<std::array<Index, rank>>;
            hpx::shared_future<std::array<CellsIdxs, nr_neighbours<rank>()>> input_cells_idxs_f{};
            hpx::future<CellsIdxs> output_cells_idxs_f{};
            hpx::tie(input_cells_idxs_f, output_cells_idxs_f) = connectivity(
                policies, flow_direction_partition, std::move(inflow_count_communicator));

            // Calculate inflow count of each cell
            using CountPartition = ArrayPartition<CountElement, rank>;
            CountPartition count_partition = inflow_count3<CountElement>(
                policies, flow_direction_partition, input_cells_idxs_f);

            return hpx::make_tuple(
                std::move(count_partition), std::move(input_cells_idxs_f), std::move(output_cells_idxs_f));
        }


        template<
            typename CountElement,
            typename Policies,
            typename FlowDirectionElement,
            Rank rank>
        struct InflowCountAction3:
            hpx::actions::make_action<
                decltype(&inflow_count3_action<CountElement, Policies, FlowDirectionElement, rank>),
                &inflow_count3_action<CountElement, Policies, FlowDirectionElement, rank>,
                InflowCountAction3<CountElement, Policies, FlowDirectionElement, rank>>::type
        {};

    }  // namespace detail


    template<
        typename CountElement,
        typename Policies,
        typename FlowDirectionElement,
        Rank rank>
    PartitionedArray<CountElement, rank> inflow_count3(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, rank> const& flow_direction)
    {
        // The result of this function must be equal to
        // upstream(flow_direction, material=1), but it should be faster
        // (less memory accesses)

        Localities<rank> const& localities{flow_direction.localities()};
        auto const& shape_in_partitions{flow_direction.partitions().shape()};

        using InflowCountCommunicator = detail::InflowCountCommunicator<rank>;
        using InflowCountCommunicatorArray = detail::CommunicatorArray<InflowCountCommunicator, rank>;

        static std::size_t call_id{0};
        InflowCountCommunicatorArray inflow_count_communicators{
            "inflow_count/" + std::to_string(call_id), localities};
        ++call_id;

        // For each partition, spawn a task that will solve the
        // calculation for the partition.
        using InflowCountArray = PartitionedArray<CountElement, rank>;
        using InflowCountPartitions = PartitionsT<InflowCountArray>;
        InflowCountPartitions inflow_count_partitions{shape_in_partitions};
        {
            detail::InflowCountAction3<CountElement, Policies, FlowDirectionElement, rank> action{};
            Count const nr_partitions{nr_elements(shape_in_partitions)};

            for(Index p = 0; p < nr_partitions; ++p)
            {
                inflow_count_partitions[p] = hpx::get<0>(hpx::split_future(hpx::async(
                    action, localities[p], policies,
                    flow_direction.partitions()[p], inflow_count_communicators[p])));
            }
        }

        // The communicators used by the tasks calculating the flow
        // accumulation result must be kept alive until these results
        // are ready. We do this by attaching a continuation,
        // passing in the communicators. Once the results are ready,
        // these communicators go out of scope. Note that component
        // clients are reference-counted.
        hpx::when_all(inflow_count_partitions.begin(), inflow_count_partitions.end(),
            [communicators1=std::move(inflow_count_communicators)]([[maybe_unused]] auto&& partitions)
            {
                HPX_UNUSED(communicators1);
            });

        return InflowCountArray{flow_direction.shape(), localities, std::move(inflow_count_partitions)};
    }

}  // namespace lue


#define LUE_INSTANTIATE_INFLOW_COUNT3(                                             \
    Policies, CountElement, FlowDirectionElement)                                  \
                                                                                   \
    template LUE_FA_EXPORT                                                         \
    PartitionedArray<CountElement, 2> inflow_count3<                               \
            CountElement, ArgumentType<void(Policies)>, FlowDirectionElement, 2>(  \
        ArgumentType<void(Policies)> const&,                                       \
        PartitionedArray<FlowDirectionElement, 2> const&);
