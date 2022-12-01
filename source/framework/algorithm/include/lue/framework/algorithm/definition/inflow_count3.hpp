#pragma once
#include "lue/framework/algorithm/routing_operation_export.hpp"
#include "lue/framework/algorithm/inflow_count.hpp"  // inflow_count_partition_data
#include "lue/framework/algorithm/inflow_count3.hpp"
#include "lue/framework/algorithm/detail/communicator_array.hpp"
#include "lue/framework/algorithm/detail/inflow_count3.hpp"
#include "lue/macro.hpp"


namespace lue {
    namespace detail {

        // Rules to convert 1D indices received on the various
        // channels, to 2D cell indices:

        // north
        // - row = 0
        // - get col from channel
        // RowIdxConverter{0};

        // south
        // - row = extent0 - 1
        // - get col from channel
        // RowIdxConverter{extent0 - 1};

        // The row is fixed, get the column from the channels
        inline std::array<Index, 2> row_idx_converter(Index const row, Index const idx)
        {
            return std::array<Index, 2>{row, idx};
        };

        // west
        // - col = 0
        // - get row from channel
        // ColIdxConverter{0};

        // east
        // - col = extent1 - 1
        // - get row from channel
        // ColIdxConverter{extent1 - 1};

        // The col is fixed, get the row from the channels
        inline std::array<Index, 2> col_idx_converter(Index const idx, Index const col)
        {
            return std::array<Index, 2>{idx, col};
        };

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

        // The row and col are fixed, discard the value from the channel
        inline std::array<Index, 2> corner_idx_converter(Index const row, Index const col)
        {
            return std::array<Index, 2>{row, col};
        };


        class RowIdxConverter
        {

            public:

                RowIdxConverter():

                    RowIdxConverter{0}

                {
                }


                RowIdxConverter(Index const row):

                    _row{row}

                {
                }


                std::array<Index, 2> operator()(Index const idx)
                {
                    return row_idx_converter(_row, idx);
                }


            private:

                Index _row;

        };


        class ColIdxConverter
        {

            public:

                ColIdxConverter():

                    ColIdxConverter{0}

                {
                }


                ColIdxConverter(Index const col):

                    _col{col}

                {
                }


                std::array<Index, 2> operator()(Index const idx)
                {
                    return row_idx_converter(idx, _col);
                }


            private:

                Index _col;

        };


        class CornerIdxConverter
        {

            public:

                CornerIdxConverter():

                    CornerIdxConverter{0, 0}

                {
                }


                CornerIdxConverter(Index const row, Index const col):

                    _row{row}, _col{col}

                {
                }


                std::array<Index, 2> operator()([[maybe_unused]] Index const idx)
                {
                    return row_idx_converter(_row, _col);
                }


            private:

                Index _row;

                Index _col;

        };


        template<
            typename T,
            typename IdxConverter,
            Rank rank>
        std::vector<std::array<Index, rank>> monitor_cell_idx_inputs(
            hpx::lcos::channel<T> const& channel,
            IdxConverter&& idx_to_idxs,
            [[maybe_unused]] Shape<Index, rank> const& partition_shape)  // TODO remove
        {
            AnnotateFunction annotation{"monitor_cell_idx_inputs"};

            std::vector<std::array<Index, rank>> cells_idxs{};

            if(channel)
            {
                for(std::vector<Index> const& idxs: channel)
                {
                    if(idxs.empty())
                    {
                        // This was the last one
                        break;
                    }

                    // For each idx in the collection, create a set of
                    // idxs, based on the shape of the partition
                    cells_idxs.reserve(cells_idxs.size() + idxs.size());
                    std::transform(idxs.begin(), idxs.end(), std::back_inserter(cells_idxs), idx_to_idxs);
                }
            }

#ifndef NDEBUG
            auto const [extent0, extent1] = partition_shape;

            for(auto const& cell_idxs: cells_idxs)
            {
                auto const [idx0, idx1] = cell_idxs;

                lue_hpx_assert(idx0 >= 0);
                lue_hpx_assert(idx0 < extent0);
                lue_hpx_assert(idx1 >= 0);
                lue_hpx_assert(idx1 < extent1);
                lue_hpx_assert(
                    (idx0 == 0 || idx0 == extent0 - 1) ||
                    (idx1 == 0 || idx1 == extent1 - 1));
            }
#endif

            return cells_idxs;
        }


        template<
            typename Policies,
            typename FlowDirectionElement,
            Rank rank>
        hpx::tuple<
            hpx::future<std::array<std::vector<std::array<Index, rank>>, nr_neighbours<rank>()>>,
            std::array<std::vector<std::array<Index, rank>>, nr_neighbours<rank>()>>
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

            Count const nr_neighbours{detail::nr_neighbours<rank>()};

            // For each partition border:
            // - For each cell that flows into a neighbouring partition
            //     - Determine the neighbouring partition (n, ne, e, ...)
            //     - Determine 1D input cell index in neighbouring partition
            //     - Store indices of output cell
            // - Send 1D idxs of receiving cells to task managing
            //     neighbouring partition, if any

            std::array<std::vector<Index>, nr_neighbours> input_cell_idxs{};
            for(auto& cell_idxs: input_cell_idxs)
            {
                cell_idxs.reserve(20);  // Prevent first few reallocations
            }

            std::array<CellsIdxs, nr_neighbours> output_cells_idxs{};
            for(auto& cells_idxs: output_cells_idxs)
            {
                cells_idxs.reserve(20);  // Prevent first few reallocations
            }

            {
                auto const& indp{std::get<0>(policies.inputs_policies()).input_no_data_policy()};
                Index idx0, idx1;
                FlowDirectionElement flow_direction;

                if(extent1 > 2)
                {
                    // North side, excluding north-west and north-east corners
                    idx0 = 0;

                    for(idx1 = 1; idx1 < extent1 - 1; ++idx1)
                    {
                        if(!indp.is_no_data(flow_direction_data, idx0, idx1))
                        {
                            flow_direction = flow_direction_data(idx0, idx1);

                            if(flow_direction == north_west<FlowDirectionElement>)
                            {
                                output_cells_idxs[accu::Direction::north].push_back({idx0, idx1});
                                input_cell_idxs[accu::Direction::north].push_back(idx1 - 1);
                            }
                            else if(flow_direction == north<FlowDirectionElement>)
                            {
                                output_cells_idxs[accu::Direction::north].push_back({idx0, idx1});
                                input_cell_idxs[accu::Direction::north].push_back(idx1);
                            }
                            else if(flow_direction == north_east<FlowDirectionElement>)
                            {
                                output_cells_idxs[accu::Direction::north].push_back({idx0, idx1});
                                input_cell_idxs[accu::Direction::north].push_back(idx1 + 1);
                            }
                        }
                    }

                    // South side, excluding south-west and south-east corners
                    idx0 = extent0 - 1;

                    for(idx1 = 1; idx1 < extent1 - 1; ++idx1)
                    {
                        if(!indp.is_no_data(flow_direction_data, idx0, idx1))
                        {
                            flow_direction = flow_direction_data(idx0, idx1);

                            if(flow_direction == south_west<FlowDirectionElement>)
                            {
                                output_cells_idxs[accu::Direction::south].push_back({idx0, idx1});
                                input_cell_idxs[accu::Direction::south].push_back(idx1 - 1);
                            }
                            else if(flow_direction == south<FlowDirectionElement>)
                            {
                                output_cells_idxs[accu::Direction::south].push_back({idx0, idx1});
                                input_cell_idxs[accu::Direction::south].push_back(idx1);
                            }
                            else if(flow_direction == south_east<FlowDirectionElement>)
                            {
                                output_cells_idxs[accu::Direction::south].push_back({idx0, idx1});
                                input_cell_idxs[accu::Direction::south].push_back(idx1 + 1);
                            }
                        }

                    }
                }

                if(extent0 > 2)
                {
                    // West side, excluding north-west and south-west corners
                    idx1 = 0;

                    for(idx0 = 1; idx0 < extent0 - 1; ++idx0)
                    {
                        if(!indp.is_no_data(flow_direction_data, idx0, idx1))
                        {
                            flow_direction = flow_direction_data(idx0, idx1);

                            if(flow_direction == north_west<FlowDirectionElement>)
                            {
                                output_cells_idxs[accu::Direction::west].push_back({idx0, idx1});
                                input_cell_idxs[accu::Direction::west].push_back(idx0 - 1);
                            }
                            else if(flow_direction == west<FlowDirectionElement>)
                            {
                                output_cells_idxs[accu::Direction::west].push_back({idx0, idx1});
                                input_cell_idxs[accu::Direction::west].push_back(idx0);
                            }
                            else if(flow_direction == south_west<FlowDirectionElement>)
                            {
                                output_cells_idxs[accu::Direction::west].push_back({idx0, idx1});
                                input_cell_idxs[accu::Direction::west].push_back(idx0 + 1);
                            }
                        }
                    }

                    // East side, excluding north-east and south-east corners
                    idx1 = extent1 - 1;

                    for(idx0 = 1; idx0 < extent0 - 1; ++idx0)
                    {
                        if(!indp.is_no_data(flow_direction_data, idx0, idx1))
                        {
                            flow_direction = flow_direction_data(idx0, idx1);

                            if(flow_direction == north_east<FlowDirectionElement>)
                            {
                                output_cells_idxs[accu::Direction::east].push_back({idx0, idx1});
                                input_cell_idxs[accu::Direction::east].push_back(idx0 - 1);
                            }
                            else if(flow_direction == east<FlowDirectionElement>)
                            {
                                output_cells_idxs[accu::Direction::east].push_back({idx0, idx1});
                                input_cell_idxs[accu::Direction::east].push_back(idx0);
                            }
                            else if(flow_direction == south_east<FlowDirectionElement>)
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

                    if(!indp.is_no_data(flow_direction_data, idx0, idx1))
                    {
                        flow_direction = flow_direction_data(idx0, idx1);

                        if(flow_direction == south_west<FlowDirectionElement>)
                        {
                            output_cells_idxs[accu::Direction::west].push_back({idx0, idx1});
                            input_cell_idxs[accu::Direction::west].push_back(idx0 + 1);
                        }
                        else if(flow_direction == west<FlowDirectionElement>)
                        {
                            output_cells_idxs[accu::Direction::west].push_back({idx0, idx1});
                            input_cell_idxs[accu::Direction::west].push_back(idx0);
                        }
                        else if(flow_direction == north_west<FlowDirectionElement>)
                        {
                            output_cells_idxs[accu::Direction::north_west].push_back({idx0, idx1});
                            input_cell_idxs[accu::Direction::north_west].push_back(std::size_t(-1));
                        }
                        else if(flow_direction == north<FlowDirectionElement>)
                        {
                            output_cells_idxs[accu::Direction::north].push_back({idx0, idx1});
                            input_cell_idxs[accu::Direction::north].push_back(idx1);
                        }
                        else if(flow_direction == north_east<FlowDirectionElement>)
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

                    if(!indp.is_no_data(flow_direction_data, idx0, idx1))
                    {
                        flow_direction = flow_direction_data(idx0, idx1);

                        if(flow_direction == north_west<FlowDirectionElement>)
                        {
                            output_cells_idxs[accu::Direction::north].push_back({idx0, idx1});
                            input_cell_idxs[accu::Direction::north].push_back(idx1 - 1);
                        }
                        else if(flow_direction == north<FlowDirectionElement>)
                        {
                            output_cells_idxs[accu::Direction::north].push_back({idx0, idx1});
                            input_cell_idxs[accu::Direction::north].push_back(idx1);
                        }
                        else if(flow_direction == north_east<FlowDirectionElement>)
                        {
                            output_cells_idxs[accu::Direction::north_east].push_back({idx0, idx1});
                            input_cell_idxs[accu::Direction::north_east].push_back(std::size_t(-1));
                        }
                        else if(flow_direction == east<FlowDirectionElement>)
                        {
                            output_cells_idxs[accu::Direction::east].push_back({idx0, idx1});
                            input_cell_idxs[accu::Direction::east].push_back(idx0);
                        }
                        else if(flow_direction == south_east<FlowDirectionElement>)
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

                    if(!indp.is_no_data(flow_direction_data, idx0, idx1))
                    {
                        flow_direction = flow_direction_data(idx0, idx1);

                        if(flow_direction == north_east<FlowDirectionElement>)
                        {
                            output_cells_idxs[accu::Direction::east].push_back({idx0, idx1});
                            input_cell_idxs[accu::Direction::east].push_back(idx0 - 1);
                        }
                        else if(flow_direction == east<FlowDirectionElement>)
                        {
                            output_cells_idxs[accu::Direction::east].push_back({idx0, idx1});
                            input_cell_idxs[accu::Direction::east].push_back(idx0);
                        }
                        else if(flow_direction == south_east<FlowDirectionElement>)
                        {
                            output_cells_idxs[accu::Direction::south_east].push_back({idx0, idx1});
                            input_cell_idxs[accu::Direction::south_east].push_back(std::size_t(-1));
                        }
                        else if(flow_direction == south<FlowDirectionElement>)
                        {
                            output_cells_idxs[accu::Direction::south].push_back({idx0, idx1});
                            input_cell_idxs[accu::Direction::south].push_back(idx1);
                        }
                        else if(flow_direction == south_west<FlowDirectionElement>)
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

                    if(!indp.is_no_data(flow_direction_data, idx0, idx1))
                    {
                        flow_direction = flow_direction_data(idx0, idx1);

                        if(flow_direction == north_west<FlowDirectionElement>)
                        {
                            output_cells_idxs[accu::Direction::west].push_back({idx0, idx1});
                            input_cell_idxs[accu::Direction::west].push_back(idx0 - 1);
                        }
                        else if(flow_direction == west<FlowDirectionElement>)
                        {
                            output_cells_idxs[accu::Direction::west].push_back({idx0, idx1});
                            input_cell_idxs[accu::Direction::west].push_back(idx0);
                        }
                        else if(flow_direction == south_west<FlowDirectionElement>)
                        {
                            output_cells_idxs[accu::Direction::south_west].push_back({idx0, idx1});
                            input_cell_idxs[accu::Direction::south_west].push_back(std::size_t(-1));
                        }
                        else if(flow_direction == south<FlowDirectionElement>)
                        {
                            output_cells_idxs[accu::Direction::south].push_back({idx0, idx1});
                            input_cell_idxs[accu::Direction::south].push_back(idx1);
                        }
                        else if(flow_direction == south_east<FlowDirectionElement>)
                        {
                            output_cells_idxs[accu::Direction::south].push_back({idx0, idx1});
                            input_cell_idxs[accu::Direction::south].push_back(idx1 + 1);
                        }
                    }
                }

                for(accu::Direction const direction: accu::directions)
                {
                    if(communicator.has_neighbour(direction) && !input_cell_idxs[direction].empty())
                    {
                        communicator.send(direction, input_cell_idxs[direction]);
                    }
                }

                // Send empty collection as a sentinel value. The receiver
                // can assume this is the last value sent.
                communicator.send({});

                // Done sending
                communicator.close();
            }

            // For each partition border:
            // - Receive 1D idxs of receiving cells
            hpx::future<std::array<CellsIdxs, nr_neighbours>> input_cells_idxs_f{};
            {
                // For each receive channel, spawn a task that will process all
                // cell indices sent through it, until no values are to
                // be expected anymore. Convert the indices to partition
                // cell indices. Aggregate the collections returned by all tasks.
                std::array<hpx::future<CellsIdxs>, nr_neighbours> received_cells_idxs{};

                received_cells_idxs[accu::Direction::north] = hpx::async(
                    monitor_cell_idx_inputs<std::vector<Index>, RowIdxConverter, rank>,
                    communicator.receive_channel(accu::Direction::north),
                    RowIdxConverter{}, flow_direction_data.shape());
                received_cells_idxs[accu::Direction::south] = hpx::async(
                    monitor_cell_idx_inputs<std::vector<Index>, RowIdxConverter, rank>,
                    communicator.receive_channel(accu::Direction::south),
                    RowIdxConverter{extent0 - 1}, flow_direction_data.shape());

                received_cells_idxs[accu::Direction::west] = hpx::async(
                    monitor_cell_idx_inputs<std::vector<Index>, ColIdxConverter, rank>,
                    communicator.receive_channel(accu::Direction::west),
                    ColIdxConverter{}, flow_direction_data.shape());
                received_cells_idxs[accu::Direction::east] = hpx::async(
                    monitor_cell_idx_inputs<std::vector<Index>, ColIdxConverter, rank>,
                    communicator.receive_channel(accu::Direction::east),
                    ColIdxConverter{extent1 - 1}, flow_direction_data.shape());

                received_cells_idxs[accu::Direction::north_west] = hpx::async(
                    monitor_cell_idx_inputs<std::vector<Index>, CornerIdxConverter, rank>,
                    communicator.receive_channel(accu::Direction::north_west),
                    CornerIdxConverter{}, flow_direction_data.shape());
                received_cells_idxs[accu::Direction::north_east] = hpx::async(
                    monitor_cell_idx_inputs<std::vector<Index>, CornerIdxConverter, rank>,
                    communicator.receive_channel(accu::Direction::north_east),
                    CornerIdxConverter{0, extent1 - 1}, flow_direction_data.shape());
                received_cells_idxs[accu::Direction::south_east] = hpx::async(
                    monitor_cell_idx_inputs<std::vector<Index>, CornerIdxConverter, rank>,
                    communicator.receive_channel(accu::Direction::south_east),
                    CornerIdxConverter{extent0 - 1, extent1 - 1}, flow_direction_data.shape());
                received_cells_idxs[accu::Direction::south_west] = hpx::async(
                    monitor_cell_idx_inputs<std::vector<Index>, CornerIdxConverter, rank>,
                    communicator.receive_channel(accu::Direction::south_west),
                    CornerIdxConverter{extent0 - 1, 0}, flow_direction_data.shape());

                input_cells_idxs_f = hpx::when_all(received_cells_idxs).then(
                        hpx::unwrapping(

                                [](std::array<hpx::future<CellsIdxs>, nr_neighbours>&& idxs_fs)
                                {
                                    std::array<CellsIdxs, detail::nr_neighbours<rank>()> cells_idxs{};

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
            hpx::future<std::array<std::vector<std::array<Index, rank>>, nr_neighbours<rank>()>>>
                connectivity(
                    Policies const& policies,
                    ArrayPartition<FlowDirectionElement, rank> const& flow_direction_partition,
                    InflowCountCommunicator<rank>&& communicator)
        {
            using FlowDirectionPartition = ArrayPartition<FlowDirectionElement, rank>;

            return hpx::split_future(hpx::dataflow(
                        hpx::launch::async,

                        [policies, communicator=std::move(communicator)](
                            FlowDirectionPartition const& flow_direction_partition) mutable
                        {
                            AnnotateFunction annotation{"connectivity"};

                            return connectivity_ready(
                                policies, flow_direction_partition, std::move(communicator));
                        },

                    flow_direction_partition));
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
            [[maybe_unused]] auto const& partition_shape{flow_direction_data.shape()};
            [[maybe_unused]] auto const [extent0, extent1] = partition_shape;
            [[maybe_unused]] auto const& indp{std::get<0>(policies.inputs_policies()).input_no_data_policy()};
            [[maybe_unused]] auto const& ondp{std::get<0>(policies.outputs_policies()).output_no_data_policy()};

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

            // Once both the flow direction partition and the input
            // cells idxs are available, forward to the function that
            // determines the inflow count. That function does two things:
            // determine inflow counts based on flow directions, and merge
            // the information about input cells. This could be split
            // into to tasks, but experiments show that the result scales
            // worse. Apparantly, the resulting tasks become too small.
            // Guideline: sometimes it is better to wait on more
            // information in order to be able to do more within a single
            // task.
            return hpx::dataflow(
                    hpx::launch::async,

                    [policies](
                        FlowDirectionPartition const& flow_direction_partition,
                        hpx::shared_future<std::array<CellsIdxs, nr_neighbours<rank>()>> const&
                            input_cells_idxs_f)
                    {
                        AnnotateFunction annotation{"inflow_count"};

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
            hpx::future<std::array<std::vector<std::array<Index, rank>>, nr_neighbours<rank>()>>>
                inflow_count3_action(
                    Policies const& policies,
                    ArrayPartition<FlowDirectionElement, rank> const& flow_direction_partition,
                    InflowCountCommunicator<rank> inflow_count_communicator)
        {
            AnnotateFunction annotation{"inflow_count"};


            // Determine connectivity between this partition and the neighbouring partitions
            using CellsIdxs = std::vector<std::array<Index, rank>>;

            hpx::shared_future<std::array<CellsIdxs, nr_neighbours<rank>()>> input_cells_idxs_f{};
            hpx::future<std::array<CellsIdxs, nr_neighbours<rank>()>> output_cells_idxs_f{};

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

        using InflowCountArray = PartitionedArray<CountElement, rank>;
        Localities<rank> const& localities{flow_direction.localities()};

        // ---------------------------------------------------------------------
        // Create an array of communicators which will be used to communicate
        // inflow counts between tasks managing neighbouring partitions.
        using InflowCountCommunicatorArray =
            detail::CommunicatorArray<detail::InflowCountCommunicator<rank>, rank>;

        InflowCountCommunicatorArray inflow_count_communicators{"/lue/inflow_count3/", localities};


        // ---------------------------------------------------------------------
        // For each partition, spawn a task that will solve the
        // calculation for the partition.
        using InflowCountPartitions = PartitionsT<InflowCountArray>;
        InflowCountPartitions inflow_count_partitions{flow_direction.partitions().shape()};
        {
            detail::InflowCountAction3<CountElement, Policies, FlowDirectionElement, rank> action{};
            Count const nr_partitions{nr_elements(inflow_count_partitions.shape())};

            for(Index p = 0; p < nr_partitions; ++p)
            {
                inflow_count_partitions[p] = hpx::get<0>(hpx::split_future(hpx::async(
                    hpx::annotated_function(action, "inflow_count"), localities[p], policies,
                    flow_direction.partitions()[p], inflow_count_communicators[p])));
            }
        }


        // ---------------------------------------------------------------------
        // Keep channel components layered in communicators alive until
        // the results are ready. Once they are, free up AGAS resources.
        hpx::when_all(inflow_count_partitions.begin(), inflow_count_partitions.end(),
            [
                inflow_count_communicators=std::move(inflow_count_communicators)
            ]([[maybe_unused]] auto&& partitions) mutable
            {
                HPX_UNUSED(inflow_count_communicators);

                auto f1{inflow_count_communicators.unregister()};

                hpx::wait_all(f1);
            });


        return InflowCountArray{flow_direction.shape(), localities, std::move(inflow_count_partitions)};
    }

}  // namespace lue


#define LUE_INSTANTIATE_INFLOW_COUNT3(                                             \
    Policies, CountElement, FlowDirectionElement)                                  \
                                                                                   \
    template LUE_ROUTING_OPERATION_EXPORT                                          \
    PartitionedArray<CountElement, 2> inflow_count3<                               \
            CountElement, ArgumentType<void(Policies)>, FlowDirectionElement, 2>(  \
        ArgumentType<void(Policies)> const&,                                       \
        PartitionedArray<FlowDirectionElement, 2> const&);
