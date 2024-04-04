#pragma once
#include "lue/framework/algorithm/clump.hpp"
#include "lue/framework/algorithm/zonal_operation_export.hpp"
#include "lue/framework/core/component.hpp"
#include "lue/macro.hpp"

#include <hpx/serialization.hpp>

#include <stack>
#include <vector>


namespace lue {
    namespace detail::clump {

        template<
            typename Array>
        auto in_clump(Array const& zone_elements, Index const row, Index const col, ElementT<Array> const zone_id) -> bool
        {
            return zone_elements(row, col) == zone_id;
        }


        template<
            typename Array>
        void add_to_clump(Array& clump_elements, Index const row, Index const col, ElementT<Array> const& clump_id)
        {
            clump_elements(row, col) = clump_id;
        }


        // Span request definition:
        // - west:         left-most col of span of cells in neighbouring row which are part of the clump
        // - east:         right-most col of span of cells in neighbouring row which are part of the clump
        // - row:          current row
        // - direction:    y-direction: -1: upwards, +1: downwards
        // using SpanRequest = std::array<Index, 4>;

        class SpanRequest
        {
            public:

                Index west;
                Index east;
                Index row;
                Index direction;
        };


        /*!
            @brief      Detect which cells are part of the clump at @a cell_idxs in @a
                        zone_elements, and mark it as clump @a clump in clump_elements
            @param      .
            @return     .
            @exception  .
        */
        template<
            typename InputNoDataPolicy,
            typename Array>
        void flood_fill(
            InputNoDataPolicy const& indp,
            Array const& zone_elements,
            std::tuple<Index, Index> const& cell_idxs,
            Array& clump_elements,
            ElementT<Array> const clump_id)
        {
            using ZoneElement = ElementT<Array>;

            assert(rank<Array> == 2);
            assert(zone_elements.shape() == clump_elements.shape());

            auto const [nr_rows, nr_cols] = zone_elements.shape();
            assert(nr_rows > 0 && nr_cols > 0);

            auto const [row, col] = cell_idxs;
            assert(!indp.is_no_data(zone_elements, row, col));

            ZoneElement const zone_id{zone_elements(row, col)};
            assert(in_clump(zone_elements, row, col, zone_id));

            std::stack<SpanRequest> requests{};

            // Push span request definitions to the stack. One for moving in upwards direction
            // and one for moving in downwards direction.
            requests.push(SpanRequest{.west=col, .east=col, .row=row - 1, .direction=-1});  // upwards
            requests.push(SpanRequest{.west=col, .east=col, .row=row, .direction=1});  // downwards

            auto row_available = [nr_rows](Index const row, Index const direction) -> bool
            {
                return
                    (direction == -1 && row > 0) ||  // upwards
                    (direction == 1 && row < nr_rows - 1);  // downwards
            };

            while(!requests.empty())
            {
                SpanRequest request{requests.top()};
                requests.pop();

                auto [west, east, row, direction] = request;

                Index col = west;

                if(!indp.is_no_data(zone_elements, row, col) && in_clump(zone_elements, row, col, zone_id))
                {
                    // Current cell is in the clump
                    // Adding it to the clump happens in the while loop below

                    // Scan cells to the left of the west cell of the span in the previous row
                    // (exclude this west cell). Add cells that are part of the clump to the clump.
                    // TODO: This loop is skipped in case current cell is not in the
                    //       clump. Does this mean this is flood fill with 4 directions?
                    //       If so, try without the condition?
                    while(col > 0 && !indp.is_no_data(zone_elements, row, col - 1) && in_clump(zone_elements, row, col - 1, zone_id))
                    {
                        add_to_clump(clump_elements, row, col - 1, clump_id);
                        col -= 1;
                    }

                    if(col < west)
                    {
                        // Found cells to the left of the west cell we started with. Push span
                        // request definition to stack, for continuing moving in the opposite
                        // direction (go back to previous row).
                        if(row_available(row, -direction))
                        {
                            requests.push(SpanRequest{.west = col, .east = west - 1, .row = row - direction, .direction = -direction});
                        }
                    }
                }

                while(west <= east)
                {
                    // Scan cells to the right of the west cell of the span in the previous row
                    // (include this west cell). Add cells that are part of the clump to the clump.
                    while(west < nr_cols - 1 && !indp.is_no_data(zone_elements, row, west) && in_clump(zone_elements, row, west, zone_id))
                    {
                        add_to_clump(clump_elements, row, west, clump_id);
                        west += 1;
                    }

                    if(west > col)
                    {
                        // Found cells to the right of the west cell we started with. Push span
                        // request definition to stack, for continuing moving in the same direction.
                        if(row_available(row, direction))
                        {
                            requests.push(SpanRequest{.west = col, .east = west - 1, .row = row + direction, .direction = direction});
                        }
                    }

                    if(west - 1 > east)
                    {
                        // Found clump cell(s) to the right of the east clump cell in the
                        // previous row. Push span request definition to stack, for continuing
                        // moving in the opposite direction.
                        if(row_available(row, -direction))
                        {
                            requests.push(SpanRequest{.west = east + 1, .east = west - 1, .row = row - direction, .direction = -direction});
                        }
                    }

                    // If we end up to the left of east, we are still within the span of
                    // the previous row. All clump cells we may encounter are still connected
                    // to the "parent"'s clump.

                    // Move to at least one cell to the right of the current cell
                    west += 1;

                    // While scanning to the right, skip all no-data and non-clump cells
                    while(west < east && (indp.is_no_data(zone_elements, row, west) || !in_clump(zone_elements, row, west, zone_id)))
                    {
                        west += 1;
                    }

                    // Set column of current cell to the first clump cell found. If no such
                    // cell was found, west == east by now
                    col = west;
                }
            }
        }


        template<typename Partition>
        class LocalResult
        {

            public:

                using ClumpElement = ElementT<Partition>;

                using ClumpIDs = std::vector<ClumpElement>;

                using ClumpData = ArrayPartitionData<ClumpElement, rank<Partition>>;

                using ZoneElement = ElementT<Partition>;

                using ZoneData = ArrayPartitionData<ZoneElement, rank<Partition>>;

                using ZoneSides = std::array<std::array<ZoneData, 2>, 2>;

                using LUT = std::vector<std::array<ClumpElement, 2>>;


                enum Direction { horizontal, vertical };
                enum HorizontalSide { north, south };
                enum VerticalSide { west, east };


                template<typename Offset>
                LocalResult(Offset const& offset, ClumpData&& data, ClumpIDs&& clump_ids, ZoneSides&& zone_sides):

                    _partition{hpx::find_here(), offset, std::move(data)},
                    _local_ids{std::move(clump_ids)},
                    _zone_sides{std::move(zone_sides)},
                    _lut{}

                {
                    // Two rows, north and south sides
                    assert(zone_sides[Direction::horizontal][HorizontalSide::north].shape()[0] <= 1);
                    assert(zone_sides[Direction::horizontal][HorizontalSide::south].shape()[0] <= 1);
                    assert(
                        zone_sides[Direction::horizontal][HorizontalSide::north].shape() ==
                        zone_sides[Direction::horizontal][HorizontalSide::south].shape());

                    // // Two columns, west and east sides
                    assert(zone_sides[Direction::vertical][VerticalSide::west].shape()[1] <= 1);
                    assert(zone_sides[Direction::vertical][VerticalSide::east].shape()[1] <= 1);
                    assert(
                        zone_sides[Direction::vertical][VerticalSide::west].shape() ==
                        zone_sides[Direction::vertical][VerticalSide::east].shape());
                }


                LocalResult() = default;

                LocalResult(LocalResult const&) = default;

                LocalResult(LocalResult&&) noexcept = default;

                ~LocalResult() = default;

                auto operator=(LocalResult const&) -> LocalResult& = default;

                auto operator=(LocalResult&&) noexcept -> LocalResult& = default;

                // auto local_ids() const -> ClumpIDs const&
                // {
                //     return _local_ids;
                // }

                auto initialize_lut(ClumpElement const initial_id) -> Count
                {
                    Count const nr_local_ids{this->nr_local_ids()};

                    _lut.resize(nr_local_ids);

                    ClumpElement global_id{initial_id};

                    for(Index idx = 0; idx < nr_local_ids; ++idx)
                    {
                        auto& record{_lut[idx]};
                        std::get<0>(record) = _local_ids[idx];
                        std::get<1>(record) = global_id++;
                    }

                    return nr_local_ids;
                }

                // auto data() -> typename Partition::Data
                // {
                //     assert(_partition.is_ready());

                //     return ready_component_ptr(_partition)->data();
                // }


                auto north_side() const -> ZoneData const&
                {
                    return _zone_sides[Direction::horizontal][HorizontalSide::north];
                }


                auto south_side() const -> ZoneData const&
                {
                    return _zone_sides[Direction::horizontal][HorizontalSide::south];
                }


                // auto partition() -> Partition
                // {
                //     // Call this function exactly once!
                //     return _partition_promise.get_future();
                // }


                // - Collection of local clump IDs
                // - Zones for all partition sides
                // - Local clump IDs associated with these zones


            private:


                auto nr_local_ids() const -> Count
                {
                    return std::size(_local_ids);
                }


                // void finish_partition()
                // {
                //     // Call this function exactly once!
                //     lue_hpx_assert(_partition.valid());
                //     lue_hpx_assert(_partition.is_ready());

                //     _partition_promise.set_value(_partition.get_id());
                // }


                // hpx::promise<hpx::id_type> _partition_promise;

                // typename Partition::Server _partition_server;


                friend class hpx::serialization::access;


                template<typename Archive>
                void serialize(Archive& archive, [[maybe_unused]] unsigned int const version)
                {
                    // clang-format off
                    archive & _partition & _local_ids & _zone_sides & _lut;
                    // clang-format on
                }


                Partition _partition;

                ClumpIDs _local_ids;

                ZoneSides _zone_sides;

                LUT _lut;
        };


        template<typename Partition>
        using LocalResults = Array<LocalResult<Partition>, rank<Partition>>;

        template<typename Partition>
        using LocalResultsF = hpx::future<Array<LocalResult<Partition>, rank<Partition>>>;

        template<typename Partition>
        using LocalResultF = hpx::future<LocalResult<Partition>>;

        template<typename Partition>
        using LocalResultFs = Array<LocalResultF<Partition>, rank<Partition>>;


        template<typename Policies, typename Partition>
        auto clump_partition(
            Policies const& policies,
            Partition const& zone_partition) -> LocalResultF<Partition>
        {
            assert(zone_partition.is_ready());

            using LocalResult = LocalResult<Partition>;
            using ClumpElement = typename LocalResult::ClumpElement;
            using ClumpData = typename LocalResult::ClumpData;
            using ClumpIDs = typename LocalResult::ClumpIDs;
            using ZoneData = typename LocalResult::ZoneData;
            using ZoneSides = typename LocalResult::ZoneSides;

            // Iterate over all cells that have not yet been identified as being part of a
            // clump and execute the flood-fill algorithm for each of these. Start with a result
            // partition that is filled with no-data. The flood-fill algorithm will write clump
            // IDs to the result partition. No-data in the result partition must exactly correspond
            // with no-data in the input partition. No more or less no-data elements.

            return hpx::dataflow(
                    hpx::launch::async,
                    hpx::unwrapping(
                        [policies, zone_partition](
                            auto const& offset,
                            auto const& zone_data) -> LocalResult
                        {
                            HPX_UNUSED(zone_partition);

                            auto const& indp = std::get<0>(policies.inputs_policies()).input_no_data_policy();
                            auto const& ondp = std::get<0>(policies.outputs_policies()).output_no_data_policy();

                            auto const shape = zone_data.shape();
                            ClumpElement clump_nd{};
                            ondp.mark_no_data(clump_nd);
                            ClumpData clump_data{shape, clump_nd};

                            auto const [nr_rows, nr_cols] = shape;
                            ClumpIDs clump_ids{};
                            ClumpElement clump_id{0};

                            for(Count row = 0; row < nr_rows; ++row)
                            {
                                for(Count col = 0; col < nr_cols; ++col)
                                {
                                    if(!indp.is_no_data(zone_data, row, col) && !ondp.is_no_data(clump_data, row, col))
                                    {
                                        flood_fill(indp, zone_data, {row, col}, clump_data, clump_id++);
                                        clump_ids.push_back(clump_id);
                                    }
                                }
                            }

                            ZoneData north{{1, nr_cols}};
                            ZoneData south{{1, nr_cols}};
                            {
                                auto src_begin = zone_data.begin();
                                std::copy(src_begin, src_begin + nr_cols, north.begin());

                                src_begin = zone_data.begin() + (nr_rows - 1) * nr_cols;
                                std::copy(src_begin, src_begin + nr_cols, south.begin());
                            }

                            ZoneData west{{nr_rows, 1}};
                            ZoneData east{{nr_rows, 1}};

                            for(Index row = 0; row < nr_rows; ++row)
                            {
                                west[row] = zone_data(row, 0);
                                east[row] = zone_data(row, nr_cols - 1);
                            }

                            ZoneSides zone_sides{{
                                {std::move(north), std::move(south)},
                                {std::move(west), std::move(east)}}};

                            return {offset, std::move(clump_data), std::move(clump_ids), std::move(zone_sides)};
                        }),
                        zone_partition.offset(),
                        zone_partition.data()
                );
        }


        template<typename Policies, typename Partition>
        struct ClumpPartitionAction:
            hpx::actions::make_action<
                decltype(&clump_partition<Policies, Partition>),
                &clump_partition<Policies, Partition>,
                ClumpPartitionAction<Policies, Partition>>::type
        {
        };


        template<typename Policies, Rank rank>
        auto solve_clump_locally(
            Policies const& policies,
            PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& zone)
                -> LocalResultFs<PartitionT<PartitionedArray<policy::OutputElementT<Policies, 0>, rank>>>
        {
            using ZoneArray = PartitionedArray<policy::InputElementT<Policies, 0>, rank>;
            using ZonePartitions = PartitionsT<ZoneArray>;
            using ZonePartition = PartitionT<ZoneArray>;

            using ClumpArray = PartitionedArray<policy::OutputElementT<Policies, 0>, rank>;
            using ClumpPartition = PartitionT<ClumpArray>;

            ClumpPartitionAction<Policies, ClumpPartition> action{};

            Localities<rank> const& localities{zone.localities()};
            ZonePartitions const& zone_partitions{zone.partitions()};
            auto const& shape_in_partitions = zone_partitions.shape();
            Count const nr_partitions{lue::nr_partitions(zone_partitions)};

            // Iterate over all partitions and asynchronously spawn a task which locally solves
            // the clump algorithm for each individual clump partition.
            detail::clump::LocalResultFs<ClumpPartition> local_result_fs(shape_in_partitions);

            {
                for(Index partition_idx = 0; partition_idx < nr_partitions; ++partition_idx)
                {
                    local_result_fs[partition_idx] = hpx::dataflow(
                        hpx::launch::async,
                        [locality_id=localities[partition_idx], action, policies](ZonePartition const& zone_partition)
                        {
                            return action(locality_id, policies, zone_partition);
                        },
                        zone_partitions[partition_idx]);
                }
            }

            return local_result_fs;
        }


        class ReclassTable
        {
        };


        // using ReclassTableFs = std::vector<hpx::future<ReclassTable>>;

        using ReclassTables = std::vector<ReclassTable>;

        using ReclassTablesF = hpx::future<ReclassTables>;


        template<typename Partition>
        auto determine_reclass_tables(LocalResultFs<Partition>&& local_result_fs) -> std::tuple<LocalResultsF<Partition>, ReclassTablesF>
        {

            using ZoneData = typename LocalResult<Partition>::ZoneData;

            return hpx::split_future(hpx::when_all(local_result_fs).then(
                hpx::unwrapping(
                    [](auto&& local_result_fs) -> std::tuple<LocalResults<Partition>, ReclassTables>
                    {
                        LocalResults<Partition> local_results{local_result_fs.shape()};
                        Count max_nr_global_ids{0};
                        Count const nr_partitions{nr_elements(local_result_fs.shape())};

                        // Initialize LUT per partition and result collection of local results
                        for(Index partition_idx = 0; partition_idx < nr_partitions; ++partition_idx)
                        {
                            auto local_result{local_result_fs[partition_idx].get()};
                            max_nr_global_ids += local_result.initialize_lut(max_nr_global_ids);
                            local_results[partition_idx] = std::move(local_result);
                        }

                        // Determine clump IDs for cells in neighbouring partition borderѕ
                        auto const [nr_rows, nr_cols] = local_results.shape();

                        if(nr_rows > 1)
                        {
                            for(Index row = 0; row < nr_rows - 1; ++row)
                            {
                                for(Index col = 0; col < nr_cols; ++col)
                                {
                                    auto& local_result1{local_results(row, col)};
                                    auto& local_result2{local_results(row + 1, col)};

                                    ZoneData const& row1{local_result1.south_side()};
                                    ZoneData const& row2{local_result2.north_side()};

                                    assert(row1.shape()[0] == 1);
                                    assert(row2.shape()[0] == 1);
                                    assert(row1.shape()[1] == row2.shape()[1]);

                                    auto const partition_nr_cols{row1.shape()[1]};

                                    ZoneData zone_data{{2, partition_nr_cols}};

                                    std::copy(row1.begin(), row1.end(), zone_data.begin());
                                    std::copy(row2.begin(), row2.end(), zone_data.begin() + partition_nr_cols);

                                    // TODO hier verder
                                    // flood_fill(indp, zone_data, clump_data, initial_clump_id);

                                    // flood_fill(indp, zone_data, {row, col}, clump_data, clump_id++);


                                }
                            }
                        }


                        // Determine clump IDs for cells in neighbouring partition corners


                        // return std::make_tuple<LocalResults<Partition>, ReclassTables>(std::move(local_results), {});
                        return {std::move(local_results), {}};
                    })));
        }


        template<
            typename Partitions>
        auto solve_clump_globally(
            LocalResultsF<PartitionT<Partitions>>&& local_results_f, ReclassTablesF&& reclass_tables_f)
                -> Partitions
        {
            // // Iterate over all partitions and asynchronously spawn a task which reclassifies the
            // // local clump IDs into global clump IDs
            // ClumpPartitions clump_partitions{zone_partitions.shape()};
            // {
            //     for(Index partition_idx = 0; partition_idx < nr_partitions; ++partition_idx)
            //     {
            //         clump_partitions[partition_idx] = detail::clump::solve_clump_globally(
            //             local_results_f[partition_idx].partition, reclass_tables_f[partition_idx]);
            //     }
            // }

            return {};
        }

    }  // namespace detail::clump


    template<typename Policies, Rank rank>
    auto clump(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& zone)
            -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank>
    {
        static_assert(std::is_same_v<policy::InputElementT<Policies, 0>, policy::OutputElementT<Policies, 0>>);
        static_assert(rank == 2);

        using ClumpArray = PartitionedArray<policy::OutputElementT<Policies, 0>, rank>;
        using ClumpPartitions = PartitionsT<ClumpArray>;

        // The algorithm has three steps:
        // 1. Solve the clump algorithm locally, for each partition (concurrent, on worker
        //    locality)
        // 2. Create reclass table per partition, for translating local clump IDs to global
        //    clump IDs (at least partly serial, on root locality)
        // 3. Reclass local clump IDs to global clump IDs (concurrent, on worker locality)
        auto local_result_fs = detail::clump::solve_clump_locally(policies, zone);
        auto [local_results_f, reclass_tables_f] = detail::clump::determine_reclass_tables(std::move(local_result_fs));
        auto clump_partitions = detail::clump::solve_clump_globally<ClumpPartitions>(
            std::move(local_results_f), std::move(reclass_tables_f));

        return {shape(zone), zone.localities(), std::move(clump_partitions)};
    }

}  // namespace lue


#define LUE_INSTANTIATE_CLUMP(Policies, rank)                                                           \
                                                                                                             \
    template LUE_ZONAL_OPERATION_EXPORT PartitionedArray<policy::OutputElementT<Policies, 0>, rank>          \
    clump<ArgumentType<void(Policies)>, rank>(                                                          \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<policy::InputElementT<Policies, 0>, rank> const&);
