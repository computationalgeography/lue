#pragma once
#include "lue/framework/algorithm/detail/flow_direction.hpp"
#include "lue/framework/algorithm/detail/halo_partition.hpp"
#include "lue/framework/algorithm/detail/partition.hpp"
#include "lue/framework/algorithm/detail/spawn.hpp"
#include "lue/framework/algorithm/detail/when_all_get.hpp"
#include "lue/framework/algorithm/flow_direction.hpp"
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/core/annotate.hpp"
#include "lue/framework/core/component.hpp"


namespace lue {
    namespace detail {

        template<typename CountElement, typename Policies, typename FlowDirectionElement, Rank rank>
        ArrayPartitionData<CountElement, rank> inflow_count_partition_data(
            Policies const& policies,
            ArrayPartitionData<FlowDirectionElement, rank> const& flow_direction_data)
        {
            // Only(!) handle the cells whose downstream cells are
            // contained within this partition.
            auto const& partition_shape{flow_direction_data.shape()};
            auto const [nr_elements0, nr_elements1] = partition_shape;

            ArrayPartitionData<CountElement, rank> inflow_count_data{partition_shape, 0};

            auto const& indp{std::get<0>(policies.inputs_policies()).input_no_data_policy()};
            auto const& ondp{std::get<0>(policies.outputs_policies()).output_no_data_policy()};

            for (Index idx0 = 1; idx0 < nr_elements0 - 1; ++idx0)
            {
                for (Index idx1 = 1; idx1 < nr_elements1 - 1; ++idx1)
                {
                    if (indp.is_no_data(flow_direction_data, idx0, idx1))
                    {
                        ondp.mark_no_data(inflow_count_data, idx0, idx1);
                    }
                    else
                    {
                        auto const [rd, cd] = downstream_cell(flow_direction_data, idx0, idx1);

                        if (rd != idx0 || cd != idx1)
                        {
                            // Current cell is not a sink. In
                            // a valid flow direction network,
                            // the destination cell cannot contain
                            // no-data.
                            lue_hpx_assert(!indp.is_no_data(flow_direction_data(rd, cd)));
                            inflow_count_data(rd, cd) += 1;
                        }
                    }
                }
            }

            // North-west cell
            {
                Index const idx0{0};
                Index const idx1{0};

                if (indp.is_no_data(flow_direction_data, idx0, idx1))
                {
                    ondp.mark_no_data(inflow_count_data, idx0, idx1);
                }
                else
                {
                    auto const flow_direction{flow_direction_data(idx0, idx1)};

                    if (flow_direction == east<FlowDirectionElement>)
                    {
                        lue_hpx_assert(!indp.is_no_data(flow_direction_data(idx0, idx1 + 1)));
                        inflow_count_data(idx0, idx1 + 1) += 1;
                    }
                    else if (flow_direction == south_east<FlowDirectionElement>)
                    {
                        lue_hpx_assert(!indp.is_no_data(flow_direction_data(idx0 + 1, idx1 + 1)));
                        inflow_count_data(idx0 + 1, idx1 + 1) += 1;
                    }
                    else if (flow_direction == south<FlowDirectionElement>)
                    {
                        lue_hpx_assert(!indp.is_no_data(flow_direction_data(idx0 + 1, idx1)));
                        inflow_count_data(idx0 + 1, idx1) += 1;
                    }
                }
            }

            // North-east cell
            {
                Index const idx0{0};
                Index const idx1{nr_elements1 - 1};

                if (indp.is_no_data(flow_direction_data, idx0, idx1))
                {
                    ondp.mark_no_data(inflow_count_data, idx0, idx1);
                }
                else
                {
                    auto const flow_direction{flow_direction_data(idx0, idx1)};

                    if (flow_direction == west<FlowDirectionElement>)
                    {
                        lue_hpx_assert(!indp.is_no_data(flow_direction_data(idx0, idx1 - 1)));
                        inflow_count_data(idx0, idx1 - 1) += 1;
                    }
                    else if (flow_direction == south_west<FlowDirectionElement>)
                    {
                        lue_hpx_assert(!indp.is_no_data(flow_direction_data(idx0 + 1, idx1 - 1)));
                        inflow_count_data(idx0 + 1, idx1 - 1) += 1;
                    }
                    else if (flow_direction == south<FlowDirectionElement>)
                    {
                        lue_hpx_assert(!indp.is_no_data(flow_direction_data(idx0 + 1, idx1)));
                        inflow_count_data(idx0 + 1, idx1) += 1;
                    }
                }
            }

            // South-west cell
            {
                Index const idx0{nr_elements0 - 1};
                Index const idx1{0};

                if (indp.is_no_data(flow_direction_data, idx0, idx1))
                {
                    ondp.mark_no_data(inflow_count_data, idx0, idx1);
                }
                else
                {
                    auto const flow_direction{flow_direction_data(idx0, idx1)};

                    if (flow_direction == north<FlowDirectionElement>)
                    {
                        lue_hpx_assert(!indp.is_no_data(flow_direction_data(idx0 - 1, idx1)));
                        inflow_count_data(idx0 - 1, idx1) += 1;
                    }
                    else if (flow_direction == north_east<FlowDirectionElement>)
                    {
                        lue_hpx_assert(!indp.is_no_data(flow_direction_data(idx0 - 1, idx1 + 1)));
                        inflow_count_data(idx0 - 1, idx1 + 1) += 1;
                    }
                    else if (flow_direction == east<FlowDirectionElement>)
                    {
                        lue_hpx_assert(!indp.is_no_data(flow_direction_data(idx0, idx1 + 1)));
                        inflow_count_data(idx0, idx1 + 1) += 1;
                    }
                }
            }

            // South-east cell
            {
                Index const idx0{nr_elements0 - 1};
                Index const idx1{nr_elements1 - 1};

                if (indp.is_no_data(flow_direction_data, idx0, idx1))
                {
                    ondp.mark_no_data(inflow_count_data, idx0, idx1);
                }
                else
                {
                    auto const flow_direction{flow_direction_data(idx0, idx1)};

                    if (flow_direction == west<FlowDirectionElement>)
                    {
                        lue_hpx_assert(!indp.is_no_data(flow_direction_data(idx0, idx1 - 1)));
                        inflow_count_data(idx0, idx1 - 1) += 1;
                    }
                    else if (flow_direction == north_west<FlowDirectionElement>)
                    {
                        lue_hpx_assert(!indp.is_no_data(flow_direction_data(idx0 - 1, idx1 - 1)));
                        inflow_count_data(idx0 - 1, idx1 - 1) += 1;
                    }
                    else if (flow_direction == north<FlowDirectionElement>)
                    {
                        lue_hpx_assert(!indp.is_no_data(flow_direction_data(idx0 - 1, idx1)));
                        inflow_count_data(idx0 - 1, idx1) += 1;
                    }
                }
            }

            if (nr_elements1 > 2)
            {
                // North cells
                {
                    Index const idx0 = 0;

                    for (Index idx1 = 1; idx1 < nr_elements1 - 1; ++idx1)
                    {
                        if (indp.is_no_data(flow_direction_data, idx0, idx1))
                        {
                            ondp.mark_no_data(inflow_count_data, idx0, idx1);
                        }
                        else
                        {
                            auto const flow_direction{flow_direction_data(idx0, idx1)};

                            if (flow_direction == west<FlowDirectionElement>)
                            {
                                lue_hpx_assert(!indp.is_no_data(flow_direction_data(idx0, idx1 - 1)));
                                inflow_count_data(idx0, idx1 - 1) += 1;
                            }
                            else if (flow_direction == south_west<FlowDirectionElement>)
                            {
                                lue_hpx_assert(!indp.is_no_data(flow_direction_data(idx0 + 1, idx1 - 1)));
                                inflow_count_data(idx0 + 1, idx1 - 1) += 1;
                            }
                            else if (flow_direction == south<FlowDirectionElement>)
                            {
                                lue_hpx_assert(!indp.is_no_data(flow_direction_data(idx0 + 1, idx1)));
                                inflow_count_data(idx0 + 1, idx1) += 1;
                            }
                            else if (flow_direction == south_east<FlowDirectionElement>)
                            {
                                lue_hpx_assert(!indp.is_no_data(flow_direction_data(idx0 + 1, idx1 + 1)));
                                inflow_count_data(idx0 + 1, idx1 + 1) += 1;
                            }
                            else if (flow_direction == east<FlowDirectionElement>)
                            {
                                lue_hpx_assert(!indp.is_no_data(flow_direction_data(idx0, idx1 + 1)));
                                inflow_count_data(idx0, idx1 + 1) += 1;
                            }
                        }
                    }
                }

                // South cells
                {
                    Index const idx0 = nr_elements0 - 1;

                    for (Index idx1 = 1; idx1 < nr_elements1 - 1; ++idx1)
                    {
                        if (indp.is_no_data(flow_direction_data, idx0, idx1))
                        {
                            ondp.mark_no_data(inflow_count_data, idx0, idx1);
                        }
                        else
                        {
                            auto const flow_direction{flow_direction_data(idx0, idx1)};

                            if (flow_direction == west<FlowDirectionElement>)
                            {
                                lue_hpx_assert(!indp.is_no_data(flow_direction_data(idx0, idx1 - 1)));
                                inflow_count_data(idx0, idx1 - 1) += 1;
                            }
                            else if (flow_direction == north_west<FlowDirectionElement>)
                            {
                                lue_hpx_assert(!indp.is_no_data(flow_direction_data(idx0 - 1, idx1 - 1)));
                                inflow_count_data(idx0 - 1, idx1 - 1) += 1;
                            }
                            else if (flow_direction == north<FlowDirectionElement>)
                            {
                                lue_hpx_assert(!indp.is_no_data(flow_direction_data(idx0 - 1, idx1)));
                                inflow_count_data(idx0 - 1, idx1) += 1;
                            }
                            else if (flow_direction == north_east<FlowDirectionElement>)
                            {
                                lue_hpx_assert(!indp.is_no_data(flow_direction_data(idx0 - 1, idx1 + 1)));
                                inflow_count_data(idx0 - 1, idx1 + 1) += 1;
                            }
                            else if (flow_direction == east<FlowDirectionElement>)
                            {
                                lue_hpx_assert(!indp.is_no_data(flow_direction_data(idx0, idx1 + 1)));
                                inflow_count_data(idx0, idx1 + 1) += 1;
                            }
                        }
                    }
                }
            }

            if (nr_elements0 > 2)
            {
                // West cells
                {
                    Index const idx1 = 0;

                    for (Index idx0 = 1; idx0 < nr_elements0 - 1; ++idx0)
                    {
                        if (indp.is_no_data(flow_direction_data, idx0, idx1))
                        {
                            ondp.mark_no_data(inflow_count_data, idx0, idx1);
                        }
                        else
                        {
                            auto const flow_direction{flow_direction_data(idx0, idx1)};

                            if (flow_direction == north<FlowDirectionElement>)
                            {
                                lue_hpx_assert(!indp.is_no_data(flow_direction_data(idx0 - 1, idx1)));
                                inflow_count_data(idx0 - 1, idx1) += 1;
                            }
                            else if (flow_direction == north_east<FlowDirectionElement>)
                            {
                                lue_hpx_assert(!indp.is_no_data(flow_direction_data(idx0 - 1, idx1 + 1)));
                                inflow_count_data(idx0 - 1, idx1 + 1) += 1;
                            }
                            else if (flow_direction == east<FlowDirectionElement>)
                            {
                                lue_hpx_assert(!indp.is_no_data(flow_direction_data(idx0, idx1 + 1)));
                                inflow_count_data(idx0, idx1 + 1) += 1;
                            }
                            else if (flow_direction == south_east<FlowDirectionElement>)
                            {
                                lue_hpx_assert(!indp.is_no_data(flow_direction_data(idx0 + 1, idx1 + 1)));
                                inflow_count_data(idx0 + 1, idx1 + 1) += 1;
                            }
                            else if (flow_direction == south<FlowDirectionElement>)
                            {
                                lue_hpx_assert(!indp.is_no_data(flow_direction_data(idx0 + 1, idx1)));
                                inflow_count_data(idx0 + 1, idx1) += 1;
                            }
                        }
                    }
                }

                // East cells
                {
                    Index const idx1 = nr_elements1 - 1;

                    for (Index idx0 = 1; idx0 < nr_elements0 - 1; ++idx0)
                    {
                        if (indp.is_no_data(flow_direction_data, idx0, idx1))
                        {
                            ondp.mark_no_data(inflow_count_data, idx0, idx1);
                        }
                        else
                        {
                            auto const flow_direction{flow_direction_data(idx0, idx1)};

                            if (flow_direction == north<FlowDirectionElement>)
                            {
                                lue_hpx_assert(!indp.is_no_data(flow_direction_data(idx0 - 1, idx1)));
                                inflow_count_data(idx0 - 1, idx1) += 1;
                            }
                            else if (flow_direction == north_west<FlowDirectionElement>)
                            {
                                lue_hpx_assert(!indp.is_no_data(flow_direction_data(idx0 - 1, idx1 - 1)));
                                inflow_count_data(idx0 - 1, idx1 - 1) += 1;
                            }
                            else if (flow_direction == west<FlowDirectionElement>)
                            {
                                lue_hpx_assert(!indp.is_no_data(flow_direction_data(idx0, idx1 - 1)));
                                inflow_count_data(idx0, idx1 - 1) += 1;
                            }
                            else if (flow_direction == south_west<FlowDirectionElement>)
                            {
                                lue_hpx_assert(!indp.is_no_data(flow_direction_data(idx0 + 1, idx1 - 1)));
                                inflow_count_data(idx0 + 1, idx1 - 1) += 1;
                            }
                            else if (flow_direction == south<FlowDirectionElement>)
                            {
                                lue_hpx_assert(!indp.is_no_data(flow_direction_data(idx0 + 1, idx1)));
                                inflow_count_data(idx0 + 1, idx1) += 1;
                            }
                        }
                    }
                }
            }

            return inflow_count_data;
        }


        template<typename InflowCountData, typename Policies, typename FlowDirectionData>
        std::tuple<InflowCountData, std::vector<std::array<Index, rank<InflowCountData>>>> inflow_count_data(
            Policies const& policies, FlowDirectionData const& flow_direction_data)
        {
            static_assert(rank<InflowCountData> == 2);

            auto const& indp{std::get<0>(policies.inputs_policies()).input_no_data_policy()};
            [[maybe_unused]] auto const& ondp{
                std::get<0>(policies.outputs_policies()).output_no_data_policy()};

            using FlowDirectionElement = ElementT<ElementT<FlowDirectionData>>;
            using Shape = ShapeT<FlowDirectionData>;

            Shape const& partition_shape{flow_direction_data(1, 1).shape()};
            auto const [nr_elements0, nr_elements1] = partition_shape;

            using CountElement = ElementT<InflowCountData>;

            InflowCountData inflow_count_data{
                inflow_count_partition_data<CountElement>(policies, flow_direction_data(1, 1))};

            // Handle all cells at the side of the center partition, whose
            // upper stream cells are located in neighbouring partitions. Besides
            // updating the inflow count of the center partition, we also keep
            // track of which cells receive input from neighbouring partitions.
            std::vector<std::array<Index, rank<InflowCountData>>> input_cell_idxs{};

            {
                // Note: all output no-data cells have already been marked in
                // the previous block

                Index idx0{};
                Index idx1{};

                Array<Shape, 2> partition_shapes{Shape{{3, 3}}};

                {
                    std::transform(
                        flow_direction_data.begin(),
                        flow_direction_data.end(),
                        partition_shapes.begin(),

                        [](auto const& data) { return data.shape(); }

                    );
                }

                // North-west corner
                {
                    auto const& data{flow_direction_data(0, 0)};

                    lue_hpx_assert(data.shape()[0] == 1);
                    lue_hpx_assert(data.shape()[1] == 1);

                    if (!indp.is_no_data(data, 0, 0))
                    {
                        auto const flow_direction{data(0, 0)};

                        if (flow_direction == south_east<FlowDirectionElement>)
                        {
                            idx0 = 0;
                            idx1 = 0;

                            lue_hpx_assert(!ondp.is_no_data(inflow_count_data, idx0, idx1));
                            inflow_count_data(idx0, idx1) += 1;
                            input_cell_idxs.push_back({idx0, idx1});
                        }
                    }
                }

                // North-east corner
                {
                    auto const& data{flow_direction_data(0, 2)};

                    lue_hpx_assert(data.shape()[0] == 1);
                    lue_hpx_assert(data.shape()[1] == 1);

                    if (!indp.is_no_data(data, 0, 0))
                    {
                        auto const flow_direction{data(0, 0)};

                        if (flow_direction == south_west<FlowDirectionElement>)
                        {
                            idx0 = 0;
                            idx1 = nr_elements1 - 1;

                            lue_hpx_assert(!ondp.is_no_data(inflow_count_data, idx0, idx1));
                            inflow_count_data(idx0, idx1) += 1;
                            input_cell_idxs.push_back({idx0, idx1});
                        }
                    }
                }

                // South-west corner
                {
                    auto const& data{flow_direction_data(2, 0)};

                    lue_hpx_assert(data.shape()[0] == 1);
                    lue_hpx_assert(data.shape()[1] == 1);

                    if (!indp.is_no_data(data, 0, 0))
                    {
                        auto const flow_direction{data(0, 0)};

                        if (flow_direction == north_east<FlowDirectionElement>)
                        {
                            idx0 = nr_elements0 - 1;
                            idx1 = 0;

                            lue_hpx_assert(!ondp.is_no_data(inflow_count_data, idx0, idx1));
                            inflow_count_data(idx0, idx1) += 1;
                            input_cell_idxs.push_back({idx0, idx1});
                        }
                    }
                }

                // South-east corner
                {
                    auto const& data{flow_direction_data(2, 2)};

                    lue_hpx_assert(data.shape()[0] == 1);
                    lue_hpx_assert(data.shape()[1] == 1);

                    if (!indp.is_no_data(data, 0, 0))
                    {
                        auto const flow_direction{data(0, 0)};

                        if (flow_direction == north_west<FlowDirectionElement>)
                        {
                            idx0 = nr_elements0 - 1;
                            idx1 = nr_elements1 - 1;

                            lue_hpx_assert(!ondp.is_no_data(inflow_count_data, idx0, idx1));
                            inflow_count_data(idx0, idx1) += 1;
                            input_cell_idxs.push_back({idx0, idx1});
                        }
                    }
                }

                // North side
                {
                    auto const& data{flow_direction_data(0, 1)};

                    lue_hpx_assert(data.shape()[0] == 1);
                    lue_hpx_assert(data.shape()[1] == nr_elements1);

                    Index const idx0 = 0;

                    {
                        Index const idx1 = 0;

                        if (!indp.is_no_data(data, 0, idx1))
                        {
                            auto const flow_direction{data(0, idx1)};

                            if (flow_direction == south<FlowDirectionElement>)
                            {
                                lue_hpx_assert(!ondp.is_no_data(inflow_count_data, idx0, idx1));
                                inflow_count_data(idx0, idx1) += 1;
                                input_cell_idxs.push_back({idx0, idx1});
                            }
                            else if (flow_direction == south_east<FlowDirectionElement>)
                            {
                                lue_hpx_assert(!ondp.is_no_data(inflow_count_data, idx0, idx1 + 1));
                                inflow_count_data(idx0, idx1 + 1) += 1;
                                input_cell_idxs.push_back({idx0, idx1 + 1});
                            }
                        }
                    }

                    if (nr_elements1 > 1)
                    {
                        Index const idx1 = nr_elements1 - 1;

                        if (!indp.is_no_data(data, 0, idx1))
                        {
                            auto const flow_direction{data(0, idx1)};

                            if (flow_direction == south_west<FlowDirectionElement>)
                            {
                                lue_hpx_assert(!ondp.is_no_data(inflow_count_data, idx0, idx1 - 1));
                                inflow_count_data(idx0, idx1 - 1) += 1;
                                input_cell_idxs.push_back({idx0, idx1 - 1});
                            }
                            else if (flow_direction == south<FlowDirectionElement>)
                            {
                                lue_hpx_assert(!ondp.is_no_data(inflow_count_data, idx0, idx1));
                                inflow_count_data(idx0, idx1) += 1;
                                input_cell_idxs.push_back({idx0, idx1});
                            }
                        }
                    }

                    if (nr_elements1 > 2)
                    {
                        for (Index idx1 = 1; idx1 < nr_elements1 - 1; ++idx1)
                        {
                            if (!indp.is_no_data(data, 0, idx1))
                            {
                                auto const flow_direction{data(0, idx1)};

                                if (flow_direction == south_west<FlowDirectionElement>)
                                {
                                    lue_hpx_assert(!ondp.is_no_data(inflow_count_data, idx0, idx1 - 1));
                                    inflow_count_data(idx0, idx1 - 1) += 1;
                                    input_cell_idxs.push_back({idx0, idx1 - 1});
                                }
                                else if (flow_direction == south<FlowDirectionElement>)
                                {
                                    lue_hpx_assert(!ondp.is_no_data(inflow_count_data, idx0, idx1));
                                    inflow_count_data(idx0, idx1) += 1;
                                    input_cell_idxs.push_back({idx0, idx1});
                                }
                                else if (flow_direction == south_east<FlowDirectionElement>)
                                {
                                    lue_hpx_assert(!ondp.is_no_data(inflow_count_data, idx0, idx1 + 1));
                                    inflow_count_data(idx0, idx1 + 1) += 1;
                                    input_cell_idxs.push_back({idx0, idx1 + 1});
                                }
                            }
                        }
                    }
                }

                // South side
                {
                    auto const& data{flow_direction_data(2, 1)};

                    lue_hpx_assert(data.shape()[0] == 1);
                    lue_hpx_assert(data.shape()[1] == nr_elements1);

                    Index const idx0 = nr_elements0 - 1;

                    {
                        Index const idx1 = 0;

                        if (!indp.is_no_data(data, 0, idx1))
                        {
                            auto const flow_direction{data(0, idx1)};

                            if (flow_direction == north<FlowDirectionElement>)
                            {
                                lue_hpx_assert(!ondp.is_no_data(inflow_count_data, idx0, idx1));
                                inflow_count_data(idx0, idx1) += 1;
                                input_cell_idxs.push_back({idx0, idx1});
                            }
                            else if (flow_direction == north_east<FlowDirectionElement>)
                            {
                                lue_hpx_assert(!ondp.is_no_data(inflow_count_data, idx0, idx1 + 1));
                                inflow_count_data(idx0, idx1 + 1) += 1;
                                input_cell_idxs.push_back({idx0, idx1 + 1});
                            }
                        }
                    }

                    if (nr_elements1 > 1)
                    {
                        Index const idx1 = nr_elements1 - 1;

                        if (!indp.is_no_data(data, 0, idx1))
                        {
                            auto const flow_direction{data(0, idx1)};

                            if (flow_direction == north_west<FlowDirectionElement>)
                            {
                                lue_hpx_assert(!ondp.is_no_data(inflow_count_data, idx0, idx1 - 1));
                                inflow_count_data(idx0, idx1 - 1) += 1;
                                input_cell_idxs.push_back({idx0, idx1 - 1});
                            }
                            else if (flow_direction == north<FlowDirectionElement>)
                            {
                                lue_hpx_assert(!ondp.is_no_data(inflow_count_data, idx0, idx1));
                                inflow_count_data(idx0, idx1) += 1;
                                input_cell_idxs.push_back({idx0, idx1});
                            }
                        }
                    }

                    if (nr_elements1 > 2)
                    {
                        for (Index idx1 = 1; idx1 < nr_elements1 - 1; ++idx1)
                        {
                            if (!indp.is_no_data(data, 0, idx1))
                            {
                                auto const flow_direction{data(0, idx1)};

                                if (flow_direction == north_west<FlowDirectionElement>)
                                {
                                    lue_hpx_assert(!ondp.is_no_data(inflow_count_data, idx0, idx1 - 1));
                                    inflow_count_data(idx0, idx1 - 1) += 1;
                                    input_cell_idxs.push_back({idx0, idx1 - 1});
                                }
                                else if (flow_direction == north<FlowDirectionElement>)
                                {
                                    lue_hpx_assert(!ondp.is_no_data(inflow_count_data, idx0, idx1));
                                    inflow_count_data(idx0, idx1) += 1;
                                    input_cell_idxs.push_back({idx0, idx1});
                                }
                                else if (flow_direction == north_east<FlowDirectionElement>)
                                {
                                    lue_hpx_assert(!ondp.is_no_data(inflow_count_data, idx0, idx1 + 1));
                                    inflow_count_data(idx0, idx1 + 1) += 1;
                                    input_cell_idxs.push_back({idx0, idx1 + 1});
                                }
                            }
                        }
                    }
                }

                // West side
                {
                    auto const& data{flow_direction_data(1, 0)};

                    lue_hpx_assert(data.shape()[0] == nr_elements0);
                    lue_hpx_assert(data.shape()[1] == 1);

                    Index const idx1 = 0;

                    {
                        Index const idx0 = 0;

                        if (!indp.is_no_data(data, idx0, 0))
                        {
                            auto const flow_direction{data(idx0, 0)};

                            if (flow_direction == east<FlowDirectionElement>)
                            {
                                lue_hpx_assert(!ondp.is_no_data(inflow_count_data, idx0, idx1));
                                inflow_count_data(idx0, idx1) += 1;
                                input_cell_idxs.push_back({idx0, idx1});
                            }
                            else if (flow_direction == south_east<FlowDirectionElement>)
                            {
                                lue_hpx_assert(!ondp.is_no_data(inflow_count_data, idx0 + 1, idx1));
                                inflow_count_data(idx0 + 1, idx1) += 1;
                                input_cell_idxs.push_back({idx0 + 1, idx1});
                            }
                        }
                    }

                    if (nr_elements0 > 1)
                    {
                        Index const idx0 = nr_elements0 - 1;

                        if (!indp.is_no_data(data, idx0, 0))
                        {
                            auto const flow_direction{data(idx0, 0)};

                            if (flow_direction == north_east<FlowDirectionElement>)
                            {
                                lue_hpx_assert(!ondp.is_no_data(inflow_count_data, idx0 - 1, idx1));
                                inflow_count_data(idx0 - 1, idx1) += 1;
                                input_cell_idxs.push_back({idx0 - 1, idx1});
                            }
                            else if (flow_direction == east<FlowDirectionElement>)
                            {
                                lue_hpx_assert(!ondp.is_no_data(inflow_count_data, idx0, idx1));
                                inflow_count_data(idx0, idx1) += 1;
                                input_cell_idxs.push_back({idx0, idx1});
                            }
                        }
                    }

                    if (nr_elements0 > 2)
                    {
                        for (Index idx0 = 1; idx0 < nr_elements0 - 1; ++idx0)
                        {
                            if (!indp.is_no_data(data, idx0, 0))
                            {
                                auto const flow_direction{data(idx0, 0)};

                                if (flow_direction == north_east<FlowDirectionElement>)
                                {
                                    lue_hpx_assert(!ondp.is_no_data(inflow_count_data, idx0 - 1, idx1));
                                    inflow_count_data(idx0 - 1, idx1) += 1;
                                    input_cell_idxs.push_back({idx0 - 1, idx1});
                                }
                                else if (flow_direction == east<FlowDirectionElement>)
                                {
                                    lue_hpx_assert(!ondp.is_no_data(inflow_count_data, idx0, idx1));
                                    inflow_count_data(idx0, idx1) += 1;
                                    input_cell_idxs.push_back({idx0, idx1});
                                }
                                else if (flow_direction == south_east<FlowDirectionElement>)
                                {
                                    lue_hpx_assert(!ondp.is_no_data(inflow_count_data, idx0 + 1, idx1));
                                    inflow_count_data(idx0 + 1, idx1) += 1;
                                    input_cell_idxs.push_back({idx0 + 1, idx1});
                                }
                            }
                        }
                    }
                }

                // East side
                {
                    auto const& data{flow_direction_data(1, 2)};

                    lue_hpx_assert(data.shape()[0] == nr_elements0);
                    lue_hpx_assert(data.shape()[1] == 1);

                    Index const idx1 = nr_elements1 - 1;

                    {
                        Index const idx0 = 0;

                        if (!indp.is_no_data(data, idx0, 0))
                        {
                            auto const flow_direction{data(idx0, 0)};

                            if (flow_direction == west<FlowDirectionElement>)
                            {
                                lue_hpx_assert(!ondp.is_no_data(inflow_count_data, idx0, idx1));
                                inflow_count_data(idx0, idx1) += 1;
                                input_cell_idxs.push_back({idx0, idx1});
                            }
                            else if (flow_direction == south_west<FlowDirectionElement>)
                            {
                                lue_hpx_assert(!ondp.is_no_data(inflow_count_data, idx0 + 1, idx1));
                                inflow_count_data(idx0 + 1, idx1) += 1;
                                input_cell_idxs.push_back({idx0 + 1, idx1});
                            }
                        }
                    }

                    if (nr_elements0 > 1)
                    {
                        Index const idx0 = nr_elements0 - 1;

                        if (!indp.is_no_data(data, idx0, 0))
                        {
                            auto const flow_direction{data(idx0, 0)};

                            if (flow_direction == north_west<FlowDirectionElement>)
                            {
                                lue_hpx_assert(!ondp.is_no_data(inflow_count_data, idx0 - 1, idx1));
                                inflow_count_data(idx0 - 1, idx1) += 1;
                                input_cell_idxs.push_back({idx0 - 1, idx1});
                            }
                            else if (flow_direction == west<FlowDirectionElement>)
                            {
                                lue_hpx_assert(!ondp.is_no_data(inflow_count_data, idx0, idx1));
                                inflow_count_data(idx0, idx1) += 1;
                                input_cell_idxs.push_back({idx0, idx1});
                            }
                        }
                    }

                    if (nr_elements0 > 2)
                    {
                        for (Index idx0 = 1; idx0 < nr_elements0 - 1; ++idx0)
                        {
                            if (!indp.is_no_data(data, idx0, 0))
                            {
                                auto const flow_direction{data(idx0, 0)};

                                if (flow_direction == north_west<FlowDirectionElement>)
                                {
                                    lue_hpx_assert(!ondp.is_no_data(inflow_count_data, idx0 - 1, idx1));
                                    inflow_count_data(idx0 - 1, idx1) += 1;
                                    input_cell_idxs.push_back({idx0 - 1, idx1});
                                }
                                else if (flow_direction == west<FlowDirectionElement>)
                                {
                                    lue_hpx_assert(!ondp.is_no_data(inflow_count_data, idx0, idx1));
                                    inflow_count_data(idx0, idx1) += 1;
                                    input_cell_idxs.push_back({idx0, idx1});
                                }
                                else if (flow_direction == south_west<FlowDirectionElement>)
                                {
                                    lue_hpx_assert(!ondp.is_no_data(inflow_count_data, idx0 + 1, idx1));
                                    inflow_count_data(idx0 + 1, idx1) += 1;
                                    input_cell_idxs.push_back({idx0 + 1, idx1});
                                }
                            }
                        }
                    }
                }
            }

            std::sort(input_cell_idxs.begin(), input_cell_idxs.end());

            return std::make_tuple(std::move(inflow_count_data), input_cell_idxs);
        }


        namespace inflow_count {

            template<typename Policies, typename InflowCountPartition, typename FlowDirectionPartitions>
            InflowCountPartition inflow_count_partition(
                Policies const& policies,
                [[maybe_unused]] OffsetT<InflowCountPartition> const& partition_offset,
                FlowDirectionPartitions const& flow_direction_partitions)
            {
                using FlowDirectionPartition = PartitionT<FlowDirectionPartitions>;
                Rank const rank{lue::rank<FlowDirectionPartition>};
                using FlowDirectionData = DataT<FlowDirectionPartition>;

                using InflowCountData = DataT<InflowCountPartition>;

                using Offset = OffsetT<FlowDirectionPartition>;
                hpx::future<Offset> offset{flow_direction_partitions(1, 1).offset()};

                // Determine shapes of all partitions
                auto const partition_shapes = detail::partition_shapes(flow_direction_partitions);

                // Request the minimum amount of required data from all partitions
                auto flow_direction_data =
                    detail::partition_data(flow_direction_partitions, partition_shapes);

                return hpx::dataflow(
                    hpx::launch::async,
                    hpx::unwrapping(

                        [policies](
                            Offset offset, lue::Array<FlowDirectionData, rank> const& flow_direction_data)
                        {
                            AnnotateFunction annotation{"inflow_count"};

                            return InflowCountPartition{
                                hpx::find_here(),
                                offset,
                                std::get<0>(detail::inflow_count_data<InflowCountData>(
                                    policies, flow_direction_data))};
                        }

                        ),
                    std::move(offset),
                    when_all_get(std::move(flow_direction_data)));
            }


            template<typename Policies, typename InflowCountPartition, typename FlowDirectionPartitions>
            struct InflowCountPartitionAction:
                hpx::actions::make_action<
                    decltype(&inflow_count_partition<
                             Policies,
                             InflowCountPartition,
                             FlowDirectionPartitions>),
                    &inflow_count_partition<Policies, InflowCountPartition, FlowDirectionPartitions>,
                    InflowCountPartitionAction<Policies, InflowCountPartition, FlowDirectionPartitions>>::type
            {
            };

        }  // namespace inflow_count
    }  // namespace detail


    namespace policy::inflow_count {

        // template<
        //     typename CountElement,
        //     typename FlowDirectionElement>
        // using DefaultPolicies = policy::DefaultPolicies<
        //     AllValuesWithinDomain<FlowDirectionElement>,
        //     OutputElements<CountElement>,
        //     InputElements<FlowDirectionElement>>;

        template<typename CountElement, typename FlowDirectionElement>
        using DefaultPolicies = Policies<
            AllValuesWithinDomain<FlowDirectionElement>,
            OutputsPolicies<OutputPolicies<
                DontMarkNoData<CountElement>,
                AllValuesWithinRange<CountElement, FlowDirectionElement>>>,
            InputsPolicies<SpatialOperationInputPolicies<
                SkipNoData<FlowDirectionElement>,
                FlowDirectionHalo<FlowDirectionElement>>>>;

        template<typename CountElement, typename FlowDirectionElement>
        using DefaultValuePolicies = Policies<
            AllValuesWithinDomain<FlowDirectionElement>,
            OutputsPolicies<OutputPolicies<
                DefaultOutputNoDataPolicy<CountElement>,
                AllValuesWithinRange<CountElement, FlowDirectionElement>>>,
            InputsPolicies<SpatialOperationInputPolicies<
                DetectNoDataByValue<FlowDirectionElement>,
                FlowDirectionHalo<FlowDirectionElement>>>>;

    }  // namespace policy::inflow_count


    template<typename CountElement, typename Policies, typename FlowDirectionElement, Rank rank>
    PartitionedArray<CountElement, rank> inflow_count(
        Policies const& policies, PartitionedArray<FlowDirectionElement, rank> const& flow_direction)
    {
        // The result of this function must be equal to
        // upstream(flow_direction, material=1), but it should be faster
        // (less memory accesses)

        using FlowDirectionArray = PartitionedArray<FlowDirectionElement, rank>;
        using FlowDirectionPartitions = PartitionsT<FlowDirectionArray>;

        using InflowCountArray = PartitionedArray<CountElement, rank>;
        using InflowCountPartition = PartitionT<InflowCountArray>;
        using InflowCountPartitions = PartitionsT<InflowCountArray>;

        using Shape = ShapeT<FlowDirectionArray>;

        FlowDirectionPartitions const& flow_direction_partitions{flow_direction.partitions()};
        Localities<rank> const& localities{flow_direction.localities()};

        lue_hpx_assert(all_are_valid(flow_direction_partitions));

        Shape min_shape{};
        std::fill(min_shape.begin(), min_shape.end(), 1);

        // Create halo partitions with flow directions away from the array
        std::array<FlowDirectionPartitions, 3> const halo_partitions = detail::halo_partitions(
            std::get<0>(policies.inputs_policies()), localities, min_shape, flow_direction_partitions);

        // Attach continuations to groups of nine partitions
        using Action = detail::inflow_count::
            InflowCountPartitionAction<Policies, InflowCountPartition, FlowDirectionPartitions>;

        auto inflow_count_partitions = detail::spawn_components<InflowCountPartitions>(
            policies, Action{}, localities, halo_partitions, flow_direction_partitions);

        lue_hpx_assert(all_are_valid(inflow_count_partitions));

        return InflowCountArray{flow_direction.shape(), localities, std::move(inflow_count_partitions)};
    }


    template<typename CountElement, typename FlowDirectionElement, Rank rank>
    PartitionedArray<CountElement, rank> inflow_count(
        PartitionedArray<FlowDirectionElement, rank> const& flow_direction)
    {
        using Policies = policy::inflow_count::DefaultPolicies<CountElement, FlowDirectionElement>;

        return inflow_count<CountElement>(Policies{}, flow_direction);
    }

}  // namespace lue
