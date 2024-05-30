#pragma once
#include "lue/framework/algorithm/clump.hpp"
#include "lue/framework/algorithm/detail/promise.hpp"
#include "lue/framework/algorithm/zonal_operation_export.hpp"
#include "lue/framework/core/component.hpp"
#include "lue/macro.hpp"

#include <hpx/serialization.hpp>

#include <stack>
#include <vector>


namespace lue {
    namespace detail::clump {

        template<typename Array>
        auto in_clump(
            Array const& zone_elements, Index const row, Index const col, ElementT<Array> const zone_id)
            -> bool
        {
            return zone_elements(row, col) == zone_id;
        }


        template<typename OutputNoDataPolicy, typename Array>
        auto added_to_clump_already(
            OutputNoDataPolicy const& ondp, Array const& clump_elements, Index const row, Index const col)
            -> bool
        {
            return !ondp.is_no_data(clump_elements(row, col));
        }


        template<typename Array>
        void add_to_clump(
            Array& clump_elements, Index const row, Index const col, ElementT<Array> const& clump_id)
        {
            clump_elements(row, col) = clump_id;
        }


        // SpanRequest defines a span of cells in the parent (previous, upper or lower row)
        // *that is part of the clump*. Each cell in the current row that is connected to one of
        // these span cells is also part of the clump.
        // Span request definition:
        // - west:         Left-most col of span of cells in parent row
        // - east:         Right-most col of span of cells in parent row
        // - row:          Current row
        // - direction:    y-direction: -1: upwards (parent is below us), +1: downwards (parent
        //                 is above us)
        // using SpanRequest = std::array<Index, 4>;

        class SpanRequest
        {
            public:

                Index row;
                Index west;
                Index east;
                Index direction;
        };


#ifndef NDEBUG
        void assert_request_is_valid(
            SpanRequest const& request, auto const& clump_elements, auto const clump_id)
        {
            auto const [row, west, east, direction] = request;
            auto const [nr_rows, nr_cols] = clump_elements.shape();

            lue_hpx_assert(row >= 0);
            lue_hpx_assert(row < nr_rows);
            lue_hpx_assert(west >= 0);
            lue_hpx_assert(west < nr_cols);
            lue_hpx_assert(east < nr_cols);
            lue_hpx_assert(west <= east);
            lue_hpx_assert(direction == -1 || direction == 1);

            for (Index col = west; col <= east; ++col)
            {
                lue_hpx_assert(clump_elements(row + -direction, col) == clump_id);
            }
        }
#else
#define assert_request_is_valid(request, clump_elements, clump_id)
#endif


        template<typename InputNoDataPolicy, typename OutputNoDataPolicy, typename Array>
        void flood_fill_nondiagonal(
            InputNoDataPolicy const& indp,
            OutputNoDataPolicy const& ondp,
            Array const& zone_elements,
            std::tuple<Index, Index> const& cell_idxs,
            Array& clump_elements,
            ElementT<Array> const clump_id)
        {
            using ZoneElement = ElementT<Array>;

            lue_hpx_assert(rank<Array> == 2);
            lue_hpx_assert(zone_elements.shape() == clump_elements.shape());

            auto const [nr_rows, nr_cols] = zone_elements.shape();
            lue_hpx_assert(nr_rows > 0 && nr_cols > 0);

            auto const [row, col] = cell_idxs;
            lue_hpx_assert(!indp.is_no_data(zone_elements, row, col));

            ZoneElement const zone_id{zone_elements(row, col)};
            lue_hpx_assert(in_clump(zone_elements, row, col, zone_id));


            // TODO Move to some helper class instance
            // Use init list to work around bug in Clang
            auto row_available = [nr_rows = nr_rows](Index const current_row, Index const direction) -> bool
            {
                return (direction == -1 && current_row > 0) ||         // upwards
                       (direction == 1 && current_row < nr_rows - 1);  // downwards
            };

            auto cell_is_connected =
                [&, indp = indp, zone_element = zone_elements](Index const row, Index const col) -> bool {
                return !indp.is_no_data(zone_elements, row, col) &&
                       in_clump(zone_elements, row, col, zone_id);
            };


            std::stack<SpanRequest> requests{};

            // Push span request definitions to the stack. One for moving in upwards direction
            // and one for moving in downwards direction.

            // downwards, start with current row
            requests.push(SpanRequest{.row = row, .west = col, .east = col, .direction = 1});

            // upwards, start with row above us, if available
            if (row_available(row, -1))
            {
                requests.push(SpanRequest{.row = row - 1, .west = col, .east = col, .direction = -1});
            }

            while (!requests.empty())
            {
                SpanRequest request{requests.top()};
                requests.pop();

                auto [row, west, east, direction] = request;

                Index col = west;

                // Changed WRT floodfill algorithm on Wikipedia:
                //     Add support for diagonal cell connectivity

                // Test whether the cell to the west of the parent span is potentially connected
                if (cell_is_connected(row, col) &&
                    // col > 0 && cell_is_connected(row, col - 1) &&
                    !added_to_clump_already(ondp, clump_elements, row, col - 1))
                {
                    // Focus in this block is on cells to the west of the current span

                    // Scan cells to the left of the west cell of the current span (exclude
                    // this west cell). While scanning, add cells that are part of the clump
                    // to the clump.
                    for (; col > 0 && cell_is_connected(row, col - 1); --col)
                    {
                        lue_hpx_assert(!added_to_clump_already(ondp, clump_elements, row, col - 1));

                        add_to_clump(clump_elements, row, col - 1, clump_id);

                        lue_hpx_assert(cell_is_connected(row, col - 1));
                        lue_hpx_assert(added_to_clump_already(ondp, clump_elements, row, col - 1));
                        lue_hpx_assert(clump_elements(row, col - 1) == clump_id);
                    }

                    if (col < west - 1)
                    {
                        // Here, col is pointing to the left-most cell *that is part of the clump*
                        lue_hpx_assert(cell_is_connected(row, col));
                        lue_hpx_assert(added_to_clump_already(ondp, clump_elements, row, col));
                        lue_hpx_assert(clump_elements(row, col) == clump_id);

                        // Changed WRT floodfill algorithm on Wikipedia:
                        //     Used (col < west - 1) instead of (col < west). The direct western
                        //     neighbour of the west cell in the previous scan should already
                        //     be part of the clump.

                        // Found cells to the left of the west cell we started with. Push span
                        // request definition to stack, for continuing moving in the previous
                        // direction (U-turn).
                        if (row_available(row, -direction))
                        {
                            // Changed WRT floodfill algorithm on Wikipedia:
                            //     Used east = west - 2 instead of east = west - 1, for the
                            //     same reason as above. west - 1 in the parent row should
                            //     already be part of the clump.
                            requests.push(SpanRequest{
                                .row = row - direction,
                                .west = col,
                                .east = west - 2,
                                .direction = -direction});
                            assert_request_is_valid(requests.top(), clump_elements, clump_id);
                        }
                    }
                }

                while (west <= east)
                {
                    // Focus in this block is on cells within the parent's span and to the
                    // right of it. As long as west <= east, cells with the right zone_id are
                    // part of the clump. Cells with another zone_id should just be skipped.

                    // Scan cells to the right of the current west cell (including this west
                    // cell). Add cells that are part of the clump to the clump. It is possible
                    // that west ends up to the right of east.
                    if (!added_to_clump_already(ondp, clump_elements, row, west))  // )
                    {
                        while (west < nr_cols && cell_is_connected(row, west))
                        {
                            lue_hpx_assert(!added_to_clump_already(ondp, clump_elements, row, west));

                            add_to_clump(clump_elements, row, west, clump_id);

                            lue_hpx_assert(cell_is_connected(row, west));
                            lue_hpx_assert(added_to_clump_already(ondp, clump_elements, row, west));
                            lue_hpx_assert(clump_elements(row, west) == clump_id);

                            west += 1;
                        }
                    }

                    // By now, west is pointing to the cell outside of the row, the cell at
                    // east + 1, or a no-data cell, or an outside-of-clump cell
                    // TODO or is already connected

                    if (west > col)
                    {
                        // Found cells to the right of the western-most cell in the current
                        // span. Push span request definition to stack, for continuing moving
                        // in the same direction.
                        if (row_available(row, direction))
                        {
                            requests.push(SpanRequest{
                                .row = row + direction,
                                .west = col,
                                .east = west - 1,
                                .direction = direction});
                            assert_request_is_valid(requests.top(), clump_elements, clump_id);
                        }
                    }

                    if (west - 1 > east)
                    {
                        // Found cells to the right of the east clump cell in the parent's span.
                        // Push span request definition to stack, for continuing moving in the
                        // previous direction (U-turn).
                        // Since west > east, this is that last thing that happens in this loop.
                        if (row_available(row, -direction))
                        {
                            // Changed WRT floodfill algorithm on Wikipedia:
                            //     Used west = east + 2 instead of west = east + 1. The direct eastern
                            //     neighbour of the east cell in the previous scan should already
                            //     be part of the clump.
                            requests.push(SpanRequest{
                                .row = row - direction,
                                .west = east + 1,
                                .east = west - 1,
                                .direction = -direction});
                            assert_request_is_valid(requests.top(), clump_elements, clump_id);
                        }
                    }

                    // If we end up to the left of east, we are still within the span of
                    // the previous row. All clump cells we may encounter are still connected
                    // to the parent's clump cells.

                    // Move to at least one cell to the right of the current cell
                    west += 1;

                    // While scanning to the right, skip all no-data and non-clump cells
                    while (west <= east && !cell_is_connected(row, west))
                    {
                        west += 1;
                    }

                    // Set column of current cell to the first clump cell found, if any
                    col = west;
                }
            }
        }


        /*!
            @brief      Detect which cells are part of the clump at @a cell_idxs in @a
                        zone_elements, and mark it as clump @a clump in clump_elements
            @param      .
            @return     .
            @exception  .
        */
        template<typename InputNoDataPolicy, typename OutputNoDataPolicy, typename Array>
        void flood_fill_diagonal(
            InputNoDataPolicy const& indp,
            OutputNoDataPolicy const& ondp,
            Array const& zone_elements,
            std::tuple<Index, Index> const& cell_idxs,
            Array& clump_elements,
            ElementT<Array> const clump_id)
        {
            using ZoneElement = ElementT<Array>;

            lue_hpx_assert(rank<Array> == 2);
            lue_hpx_assert(zone_elements.shape() == clump_elements.shape());

            auto const [nr_rows, nr_cols] = zone_elements.shape();
            lue_hpx_assert(nr_rows > 0 && nr_cols > 0);

            auto const [row, col] = cell_idxs;
            lue_hpx_assert(!indp.is_no_data(zone_elements, row, col));

            ZoneElement const zone_id{zone_elements(row, col)};
            lue_hpx_assert(in_clump(zone_elements, row, col, zone_id));


            // Use init list to work around bug in Clang
            // auto row_available = [nr_rows=nr_rows](Index const current_row, Index const direction) -> bool
            auto row_available = [nr_rows = nr_rows](Index const current_row, Index const direction) -> bool
            {
                return (direction == -1 && current_row > 0) ||         // upwards
                       (direction == 1 && current_row < nr_rows - 1);  // downwards
            };

            auto cell_is_connected =
                [&, indp = indp, zone_element = zone_elements](Index const row, Index const col) -> bool {
                return !indp.is_no_data(zone_elements, row, col) &&
                       in_clump(zone_elements, row, col, zone_id);
            };


            std::stack<SpanRequest> requests{};

            // Push span request definitions to the stack. One for moving in upwards direction
            // and one for moving in downwards direction.

            // downwards, start with current row
            requests.push(SpanRequest{.row = row, .west = col, .east = col, .direction = 1});
            // assert_request_is_valid(requests.top(), clump_elements, clump_id);

            // upwards, start with row above us, if available
            if (row_available(row, -1))
            {
                requests.push(SpanRequest{.row = row - 1, .west = col, .east = col, .direction = -1});
                // assert_request_is_valid(requests.top(), clump_elements, clump_id);
            }

            while (!requests.empty())
            {
                SpanRequest request{requests.top()};
                requests.pop();

                auto [row, west, east, direction] = request;

                Index col = west;

                // Changed WRT floodfill algorithm on Wikipedia:
                //     Add support for diagonal cell connectivity

                // Test whether the cell to the west of the parent span is potentially connected
                if (col > 0 && cell_is_connected(row, col - 1) &&
                    !added_to_clump_already(ondp, clump_elements, row, col - 1))
                {
                    // Focus in this block is on cells to the west of the current span

                    // Scan cells to the left of the west cell of the current span (exclude
                    // this west cell). While scanning, add cells that are part of the clump
                    // to the clump.
                    for (; col > 0 && cell_is_connected(row, col - 1); --col)
                    {
                        lue_hpx_assert(!added_to_clump_already(ondp, clump_elements, row, col - 1));

                        add_to_clump(clump_elements, row, col - 1, clump_id);

                        lue_hpx_assert(cell_is_connected(row, col - 1));
                        lue_hpx_assert(added_to_clump_already(ondp, clump_elements, row, col - 1));
                        lue_hpx_assert(clump_elements(row, col - 1) == clump_id);
                    }

                    if (col < west - 1)
                    {
                        // Here, col is pointing to the left-most cell *that is part of the clump*
                        lue_hpx_assert(cell_is_connected(row, col));
                        lue_hpx_assert(added_to_clump_already(ondp, clump_elements, row, col));
                        lue_hpx_assert(clump_elements(row, col) == clump_id);

                        // Changed WRT floodfill algorithm on Wikipedia:
                        //     Used (col < west - 1) instead of (col < west). The direct western
                        //     neighbour of the west cell in the previous scan should already
                        //     be part of the clump.

                        // Found cells to the left of the west cell we started with. Push span
                        // request definition to stack, for continuing moving in the previous
                        // direction (U-turn).
                        if (row_available(row, -direction))
                        {
                            // Changed WRT floodfill algorithm on Wikipedia:
                            //     Used east = west - 2 instead of east = west - 1, for the
                            //     same reason as above. west - 1 in the parent row should
                            //     already be part of the clump.
                            requests.push(SpanRequest{
                                .row = row - direction,
                                .west = col,
                                .east = west - 2,
                                .direction = -direction});
                            assert_request_is_valid(requests.top(), clump_elements, clump_id);
                        }
                    }
                }

                while (west <= east)
                {
                    // Focus in this block is on cells within the parent's span and to the
                    // right of it. As long as west <= east, cells with the right zone_id are
                    // part of the clump. Cells with another zone_id should just be skipped.

                    // Scan cells to the right of the current west cell (including this west
                    // cell). Add cells that are part of the clump to the clump. It is possible
                    // that west ends up to the right of east.
                    if (!added_to_clump_already(ondp, clump_elements, row, west))
                    {
                        while (west < nr_cols && cell_is_connected(row, west))
                        {
                            lue_hpx_assert(!added_to_clump_already(ondp, clump_elements, row, west));

                            add_to_clump(clump_elements, row, west, clump_id);

                            lue_hpx_assert(cell_is_connected(row, west));
                            lue_hpx_assert(added_to_clump_already(ondp, clump_elements, row, west));
                            lue_hpx_assert(clump_elements(row, west) == clump_id);

                            west += 1;
                        }
                    }

                    // By now, west is pointing to the cell outside of the row, the cell at
                    // east + 1, or a no-data cell, or an outside-of-clump cell
                    // TODO or is already connected

                    if (west > col)
                    {
                        // Found cells to the right of the western-most cell in the current
                        // span. Push span request definition to stack, for continuing moving
                        // in the same direction.
                        if (row_available(row, direction))
                        {
                            requests.push(SpanRequest{
                                .row = row + direction,
                                .west = col,
                                .east = west - 1,
                                .direction = direction});
                            assert_request_is_valid(requests.top(), clump_elements, clump_id);
                        }
                    }

                    if (west - 1 > east)
                    {
                        // Found cells to the right of the east clump cell in the parent's span.
                        // Push span request definition to stack, for continuing moving in the
                        // previous direction (U-turn).
                        // Since west > east, this is that last thing that happens in this loop.
                        if (row_available(row, -direction))
                        {
                            // Changed WRT floodfill algorithm on Wikipedia:
                            //     Used west = east + 2 instead of west = east + 1. The direct eastern
                            //     neighbour of the east cell in the previous scan should already
                            //     be part of the clump.
                            requests.push(SpanRequest{
                                .row = row - direction,
                                .west = east + 1,
                                .east = west - 1,
                                .direction = -direction});
                            assert_request_is_valid(requests.top(), clump_elements, clump_id);
                        }
                    }

                    // If we end up to the left of east, we are still within the span of
                    // the previous row. All clump cells we may encounter are still connected
                    // to the parent's clump cells.

                    // Move to at least one cell to the right of the current cell
                    west += 1;

                    // While scanning to the right, skip all no-data and non-clump cells
                    while (west <= east && !cell_is_connected(row, west))
                    {
                        west += 1;
                    }

                    // Set column of current cell to the first clump cell found, if any
                    col = west;
                }

                // Changed WRT floodfill algorithm on Wikipedia:
                //     Add support for diagonal cell connectivity
                if (east < nr_cols - 1 && west - 1 == east && cell_is_connected(row, west) &&
                    !added_to_clump_already(ondp, clump_elements, row, west))
                {
                    while (west < nr_cols && cell_is_connected(row, west))
                    {
                        lue_hpx_assert(!added_to_clump_already(ondp, clump_elements, row, west));

                        add_to_clump(clump_elements, row, west, clump_id);

                        lue_hpx_assert(cell_is_connected(row, west));
                        lue_hpx_assert(added_to_clump_already(ondp, clump_elements, row, west));
                        lue_hpx_assert(clump_elements(row, west) == clump_id);

                        west += 1;
                    }

                    // By now, west is pointing to the cell outside of the row, the cell at
                    // east + 1, or a no-data cell, or an outside-of-clump cell

                    if (row_available(row, direction))
                    {
                        requests.push(SpanRequest{
                            .row = row + direction,
                            .west = east + 1,
                            .east = west - 1,
                            .direction = direction});
                        assert_request_is_valid(requests.top(), clump_elements, clump_id);
                    }

                    if (row_available(row, -direction))
                    {
                        requests.push(SpanRequest{
                            .row = row - direction,
                            .west = east + 1,
                            .east = west - 1,
                            .direction = -direction});
                        assert_request_is_valid(requests.top(), clump_elements, clump_id);
                    }
                }
            }
        }


        template<typename InputNoDataPolicy, typename OutputNoDataPolicy, typename Array>
        void flood_fill(
            InputNoDataPolicy const& indp,
            OutputNoDataPolicy const& ondp,
            Array const& zone_elements,
            std::tuple<Index, Index> const& cell_idxs,
            Array& clump_elements,
            ElementT<Array> const clump_id,
            Connectivity const connectivity)
        {
            switch (connectivity)
            {
                case Connectivity::nondiagonal:
                {
                    flood_fill_nondiagonal(indp, ondp, zone_elements, cell_idxs, clump_elements, clump_id);
                    break;
                }
                case Connectivity::diagonal:
                {
                    flood_fill_diagonal(indp, ondp, zone_elements, cell_idxs, clump_elements, clump_id);
                    break;
                }
            }
        }


        template<typename Partition>
        class LocalResult
        {

            public:

                using ClumpElement = ElementT<Partition>;

                using ClumpData = ArrayPartitionData<ClumpElement, rank<Partition>>;

                using ClumpSides = std::array<std::array<ClumpData, 2>, 2>;

                using ZoneElement = ElementT<Partition>;

                using ZoneData = ArrayPartitionData<ZoneElement, rank<Partition>>;

                using ZoneSides = std::array<std::array<ZoneData, 2>, 2>;

                using LUT = std::map<ClumpElement, ClumpElement>;

                enum Direction { horizontal, vertical };
                enum HorizontalSide { north, south };
                enum VerticalSide { west, east };


                template<typename Offset>
                LocalResult(
                    Offset const& offset,
                    ClumpData&& data,
                    Count const nr_clumps,
                    ZoneSides&& zone_sides,
                    ClumpSides&& clump_sides):

                    _partition{hpx::find_here(), offset, std::move(data)},
                    _zone_sides{std::move(zone_sides)},
                    _clump_sides{std::move(clump_sides)},
                    _lut{}

                {
                    // Two rows, north and south sides
                    lue_hpx_assert(zone_sides[Direction::horizontal][HorizontalSide::north].shape()[0] <= 1);
                    lue_hpx_assert(zone_sides[Direction::horizontal][HorizontalSide::south].shape()[0] <= 1);
                    lue_hpx_assert(
                        zone_sides[Direction::horizontal][HorizontalSide::north].shape() ==
                        zone_sides[Direction::horizontal][HorizontalSide::south].shape());

                    // // Two columns, west and east sides
                    lue_hpx_assert(zone_sides[Direction::vertical][VerticalSide::west].shape()[1] <= 1);
                    lue_hpx_assert(zone_sides[Direction::vertical][VerticalSide::east].shape()[1] <= 1);
                    lue_hpx_assert(
                        zone_sides[Direction::vertical][VerticalSide::west].shape() ==
                        zone_sides[Direction::vertical][VerticalSide::east].shape());

                    // Two rows, north and south sides
                    lue_hpx_assert(clump_sides[Direction::horizontal][HorizontalSide::north].shape()[0] <= 1);
                    lue_hpx_assert(clump_sides[Direction::horizontal][HorizontalSide::south].shape()[0] <= 1);
                    lue_hpx_assert(
                        clump_sides[Direction::horizontal][HorizontalSide::north].shape() ==
                        clump_sides[Direction::horizontal][HorizontalSide::south].shape());

                    // // Two columns, west and east sides
                    lue_hpx_assert(clump_sides[Direction::vertical][VerticalSide::west].shape()[1] <= 1);
                    lue_hpx_assert(clump_sides[Direction::vertical][VerticalSide::east].shape()[1] <= 1);
                    lue_hpx_assert(
                        clump_sides[Direction::vertical][VerticalSide::west].shape() ==
                        clump_sides[Direction::vertical][VerticalSide::east].shape());

                    ClumpElement clump_id{0};

                    for (Count idx = 0; idx < nr_clumps; ++idx, ++clump_id)
                    {
                        _lut[clump_id] = clump_id;
                    }
                }


                LocalResult() = default;

                LocalResult(LocalResult const&) = default;

                LocalResult(LocalResult&&) noexcept = default;

                ~LocalResult() = default;

                auto operator=(LocalResult const&) -> LocalResult& = default;

                auto operator=(LocalResult&&) noexcept -> LocalResult& = default;


                auto partition() const -> Partition
                {
                    return _partition;
                }


                auto reclassify_global_clump_ids(ClumpElement const initial_id) -> Count
                {
                    for (auto& pair : _lut)
                    {
                        pair.second += initial_id;
                    }

                    return _lut.size();
                }


                auto lut() -> LUT const&
                {
                    return _lut;
                }


                auto global_clump_id(ClumpElement const local_clump_id) const -> ClumpElement
                {
                    lue_hpx_assert(_lut.find(local_clump_id) != _lut.end());

                    return _lut.find(local_clump_id)->second;
                }


                void update_global_clump_id(
                    ClumpElement const local_clump_id, ClumpElement const new_global_clump_id)
                {
                    lue_hpx_assert(_lut.find(local_clump_id) != _lut.end());

                    _lut.find(local_clump_id)->second = new_global_clump_id;
                }


                void update_global_clump_ids(std::set<ClumpElement> const& global_clump_ids_removed)
                {
                    for (auto& [local_clump_id, global_clump_id] : _lut)
                    {
                        // Subtract the number of removed global clump IDs with a lower ID than
                        // the current global clump ID from this current global clump ID
                        auto const upper_bound = global_clump_ids_removed.upper_bound(global_clump_id);
                        global_clump_id -= std::distance(global_clump_ids_removed.begin(), upper_bound);
                    }
                }


                auto north_side() const -> std::
                    tuple<std::reference_wrapper<ZoneData const>, std::reference_wrapper<ClumpData const>>
                {
                    return {
                        std::cref(_zone_sides[Direction::horizontal][HorizontalSide::north]),
                        std::cref(_clump_sides[Direction::horizontal][HorizontalSide::north])};
                }


                auto south_side() const -> std::
                    tuple<std::reference_wrapper<ZoneData const>, std::reference_wrapper<ClumpData const>>
                {
                    return {
                        std::cref(_zone_sides[Direction::horizontal][HorizontalSide::south]),
                        std::cref(_clump_sides[Direction::horizontal][HorizontalSide::south])};
                }


                auto west_side() const -> std::
                    tuple<std::reference_wrapper<ZoneData const>, std::reference_wrapper<ClumpData const>>
                {
                    return {
                        std::cref(_zone_sides[Direction::vertical][VerticalSide::west]),
                        std::cref(_clump_sides[Direction::vertical][VerticalSide::west])};
                }


                auto east_side() const -> std::
                    tuple<std::reference_wrapper<ZoneData const>, std::reference_wrapper<ClumpData const>>
                {
                    return {
                        std::cref(_zone_sides[Direction::vertical][VerticalSide::east]),
                        std::cref(_clump_sides[Direction::vertical][VerticalSide::east])};
                }


                auto north_west_corner() const -> std::tuple<ZoneElement, ClumpElement>
                {
                    return {
                        _zone_sides[Direction::horizontal][HorizontalSide::north][0],
                        _clump_sides[Direction::horizontal][HorizontalSide::north][0]};
                }


                auto north_east_corner() const -> std::tuple<ZoneElement, ClumpElement>
                {
                    return {
                        _zone_sides[Direction::vertical][VerticalSide::east][0],
                        _clump_sides[Direction::vertical][VerticalSide::east][0]};
                }


                auto south_east_corner() const -> std::tuple<ZoneElement, ClumpElement>
                {
                    auto const nr_rows = _zone_sides[Direction::vertical][VerticalSide::east].shape()[0];
                    lue_hpx_assert(nr_rows > 0);

                    return {
                        _zone_sides[Direction::vertical][VerticalSide::east][nr_rows - 1],
                        _clump_sides[Direction::vertical][VerticalSide::east][nr_rows - 1]};
                }


                auto south_west_corner() const -> std::tuple<ZoneElement, ClumpElement>
                {
                    return {
                        _zone_sides[Direction::horizontal][HorizontalSide::south][0],
                        _clump_sides[Direction::horizontal][HorizontalSide::south][0]};
                }


            private:

                friend class hpx::serialization::access;


                template<typename Archive>
                void serialize(Archive& archive, [[maybe_unused]] unsigned int const version)
                {
                    // clang-format off
                    archive
                        & _partition
                        & _zone_sides
                        & _clump_sides
                        & _lut
                        ;
                    // clang-format on
                }


                Partition _partition;

                ZoneSides _zone_sides;

                ClumpSides _clump_sides;

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
            Policies const& policies, Partition const& zone_partition, Connectivity const connectivity)
            -> LocalResultF<Partition>
        {
            lue_hpx_assert(zone_partition.is_ready());

            using LocalResult = LocalResult<Partition>;
            using ClumpElement = typename LocalResult::ClumpElement;
            using ClumpData = typename LocalResult::ClumpData;
            using ClumpSides = typename LocalResult::ClumpSides;
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
                    [policies, zone_partition, connectivity](
                        auto const& offset, auto const& zone_data) -> LocalResult
                    {
                        HPX_UNUSED(zone_partition);

                        auto const& indp = std::get<0>(policies.inputs_policies()).input_no_data_policy();
                        auto const& ondp = std::get<0>(policies.outputs_policies()).output_no_data_policy();

                        auto const shape = zone_data.shape();
                        ClumpElement clump_nd{};
                        ondp.mark_no_data(clump_nd);
                        ClumpData clump_data{shape, clump_nd};

                        auto const [nr_rows, nr_cols] = shape;
                        ClumpElement clump_id{0};

                        for (Count row = 0; row < nr_rows; ++row)
                        {
                            for (Count col = 0; col < nr_cols; ++col)
                            {
                                // Skip cells that do not contain a valid zone ID, or that
                                // already contain a valid clump ID
                                if ((!indp.is_no_data(zone_data, row, col)) &&
                                    ondp.is_no_data(clump_data, row, col))
                                {
                                    flood_fill(
                                        indp,
                                        ondp,
                                        zone_data,
                                        {row, col},
                                        clump_data,
                                        clump_id,
                                        connectivity);
                                    ++clump_id;
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

                        for (Index row = 0; row < nr_rows; ++row)
                        {
                            west[row] = zone_data(row, 0);
                            east[row] = zone_data(row, nr_cols - 1);
                        }

                        ZoneSides zone_sides{
                            {{std::move(north), std::move(south)}, {std::move(west), std::move(east)}}};

                        ClumpData clump_data_north{{1, nr_cols}};
                        ClumpData clump_data_south{{1, nr_cols}};
                        {
                            auto src_begin = clump_data.begin();
                            std::copy(src_begin, src_begin + nr_cols, clump_data_north.begin());

                            src_begin = clump_data.begin() + (nr_rows - 1) * nr_cols;
                            std::copy(src_begin, src_begin + nr_cols, clump_data_south.begin());
                        }

                        ClumpData clump_data_west{{nr_rows, 1}};
                        ClumpData clump_data_east{{nr_rows, 1}};

                        for (Index row = 0; row < nr_rows; ++row)
                        {
                            clump_data_west[row] = clump_data(row, 0);
                            clump_data_east[row] = clump_data(row, nr_cols - 1);
                        }

                        ClumpSides clump_sides{
                            {{std::move(clump_data_north), std::move(clump_data_south)},
                             {std::move(clump_data_west), std::move(clump_data_east)}}};

                        return {
                            offset,
                            std::move(clump_data),
                            static_cast<Count>(clump_id),
                            std::move(zone_sides),
                            std::move(clump_sides)};
                    }),
                zone_partition.offset(),
                zone_partition.data());
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
            PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& zone,
            Connectivity const connectivity)
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
            Count const nr_partitions{lue::nr_elements(shape_in_partitions)};

            // Iterate over all partitions and asynchronously spawn a task which locally solves
            // the clump algorithm for each individual clump partition.
            detail::clump::LocalResultFs<ClumpPartition> local_result_fs{shape_in_partitions};

            {
                for (Index partition_idx = 0; partition_idx < nr_partitions; ++partition_idx)
                {
                    local_result_fs[partition_idx] = hpx::dataflow(
                        hpx::launch::async,
                        [locality_id = localities[partition_idx], action, policies, connectivity](
                            ZonePartition const& zone_partition)
                        { return action(locality_id, policies, zone_partition, connectivity); },
                        zone_partitions[partition_idx]);
                }
            }

            return local_result_fs;
        }


        template<typename Policies, typename Partition>
        auto determine_reclass_tables(
            Policies const& policies,
            LocalResultFs<Partition>&& local_result_fs,
            Connectivity const connectivity) -> LocalResultsF<Partition>
        {
            using ZoneData = typename LocalResult<Partition>::ZoneData;
            using ClumpData = typename LocalResult<Partition>::ClumpData;
            using ClumpElement = typename LocalResult<Partition>::ClumpElement;

            return hpx::when_all(local_result_fs.begin(), local_result_fs.end())
                .then(hpx::unwrapping(
                    [policies, shape_in_partitions = local_result_fs.shape(), connectivity](
                        auto&& local_result_fs) -> LocalResults<Partition>
                    {
                        auto const& indp = std::get<0>(policies.inputs_policies()).input_no_data_policy();
                        auto const& ondp = std::get<0>(policies.outputs_policies()).output_no_data_policy();

                        LocalResults<Partition> local_results{shape_in_partitions};
                        Count max_nr_global_ids{0};  // Actual nr is likely smaller
                        Count const nr_partitions{nr_elements(shape_in_partitions)};

                        // Initialize LUT per partition and result collection of local results
                        for (Index partition_idx = 0; partition_idx < nr_partitions; ++partition_idx)
                        {
                            local_results[partition_idx] = local_result_fs[partition_idx].get();
                            max_nr_global_ids +=
                                local_results[partition_idx].reclassify_global_clump_ids(max_nr_global_ids);
                        }

                        // Current global clump IDs from different partitions that share the
                        // same final global clumps
                        std::vector<std::set<ClumpElement>> global_clump_ids{};

                        // Per local clump in a partition the location of the collection of
                        // current global clump IDs that must be merged
                        std::map<std::tuple<Index, Index, ClumpElement>, Index>
                            global_clump_ids_by_local_clump_id{};

                        // Determine clump IDs for cells in neighbouring partition borderѕ
                        auto const [nr_rows_in_partitions, nr_cols_in_partitions] = local_results.shape();

                        if (nr_rows_in_partitions > 1)
                        {
                            for (Index row_in_partitions = 0; row_in_partitions < nr_rows_in_partitions - 1;
                                 ++row_in_partitions)
                            {
                                for (Index col_in_partitions = 0; col_in_partitions < nr_cols_in_partitions;
                                     ++col_in_partitions)
                                {
                                    auto& north_local_result{
                                        local_results(row_in_partitions, col_in_partitions)};
                                    auto& south_local_result{
                                        local_results(row_in_partitions + 1, col_in_partitions)};

                                    // Yes, the south side of the north partition is the north side
                                    // of the array we build here, etc
                                    auto const& [north_zone_ids, north_clump_ids] =
                                        north_local_result.south_side();
                                    auto const& [south_zone_ids, south_clump_ids] =
                                        south_local_result.north_side();

                                    lue_hpx_assert(north_zone_ids.get().shape()[0] == 1);
                                    lue_hpx_assert(south_zone_ids.get().shape()[0] == 1);
                                    lue_hpx_assert(
                                        north_zone_ids.get().shape()[1] == south_zone_ids.get().shape()[1]);

                                    ZoneData zone_data{{2, north_zone_ids.get().shape()[1]}};
                                    auto const [nr_rows, nr_cols] = zone_data.shape();

                                    std::copy(
                                        north_zone_ids.get().begin(),
                                        north_zone_ids.get().end(),
                                        zone_data.begin());
                                    std::copy(
                                        south_zone_ids.get().begin(),
                                        south_zone_ids.get().end(),
                                        zone_data.begin() + nr_cols);


                                    ClumpElement clump_nd{};
                                    ondp.mark_no_data(clump_nd);
                                    ClumpData clump_data{zone_data.shape(), clump_nd};

                                    {
                                        ClumpElement clump_id{0};

                                        for (Count row = 0; row < nr_rows; ++row)
                                        {
                                            for (Count col = 0; col < nr_cols; ++col)
                                            {
                                                // Skip cells that do not contain a valid zone ID, or that
                                                // already contain a valid clump ID
                                                if ((!indp.is_no_data(zone_data, row, col)) &&
                                                    ondp.is_no_data(clump_data, row, col))
                                                {
                                                    flood_fill(
                                                        indp,
                                                        ondp,
                                                        zone_data,
                                                        {row, col},
                                                        clump_data,
                                                        clump_id,
                                                        connectivity);
                                                    ++clump_id;
                                                }
                                            }
                                        }
                                    }

                                    // Find indxs of cells on both sides of the partition
                                    // boundary that belong to the same clump.
                                    // For each of those indxs, add information about the
                                    // partition_idx, local_clump_id, global_clump_id to a
                                    // collection, if not already present. Aggregate information
                                    // about the same clump.

                                    std::set<ClumpElement> clump_handled{};

                                    for (Index north_idx = 0; north_idx < nr_cols; ++north_idx)
                                    {
                                        if (!indp.is_no_data(clump_data[north_idx]) &&
                                            !clump_handled.contains(clump_data[north_idx]))
                                        {
                                            Index const south_idx = std::distance(
                                                clump_data.begin() + nr_cols,
                                                std::find(
                                                    clump_data.begin() + nr_cols,
                                                    clump_data.end(),
                                                    clump_data[north_idx]));

                                            if (south_idx != nr_cols)
                                            {
                                                // Clump extends on both sides of the partition boundary
                                                auto const north_local_clump_id{
                                                    north_clump_ids.get()[north_idx]};
                                                auto const south_local_clump_id{
                                                    south_clump_ids.get()[south_idx]};

                                                auto const north_key = std::make_tuple(
                                                    row_in_partitions,
                                                    col_in_partitions,
                                                    north_local_clump_id);
                                                auto const south_key = std::make_tuple(
                                                    row_in_partitions + 1,
                                                    col_in_partitions,
                                                    south_local_clump_id);

                                                auto north_it =
                                                    global_clump_ids_by_local_clump_id.find(north_key);
                                                auto south_it =
                                                    global_clump_ids_by_local_clump_id.find(south_key);

                                                Index north_global_clump_ids_idx{-1};
                                                Index south_global_clump_ids_idx{-1};

                                                if (north_it != global_clump_ids_by_local_clump_id.end())
                                                {
                                                    north_global_clump_ids_idx = north_it->second;

                                                    if (south_it == global_clump_ids_by_local_clump_id.end())
                                                    {
                                                        // north_it != end && south == end
                                                        south_global_clump_ids_idx =
                                                            north_global_clump_ids_idx;
                                                        global_clump_ids_by_local_clump_id[south_key] =
                                                            south_global_clump_ids_idx;
                                                    }
                                                    else
                                                    {
                                                        south_global_clump_ids_idx = south_it->second;
                                                    }
                                                }
                                                else if (south_it != global_clump_ids_by_local_clump_id.end())
                                                {
                                                    // north_it == end && south_it != end
                                                    south_global_clump_ids_idx = south_it->second;
                                                    north_global_clump_ids_idx = south_global_clump_ids_idx;
                                                    global_clump_ids_by_local_clump_id[north_key] =
                                                        north_global_clump_ids_idx;
                                                }
                                                else
                                                {
                                                    // north_it == end && south_it == end
                                                    global_clump_ids.push_back({});
                                                    north_global_clump_ids_idx = global_clump_ids.size() - 1;
                                                    south_global_clump_ids_idx = north_global_clump_ids_idx;
                                                    global_clump_ids_by_local_clump_id[north_key] =
                                                        north_global_clump_ids_idx;
                                                    global_clump_ids_by_local_clump_id[south_key] =
                                                        south_global_clump_ids_idx;
                                                }

                                                lue_hpx_assert(north_global_clump_ids_idx >= 0);
                                                lue_hpx_assert(south_global_clump_ids_idx >= 0);

                                                global_clump_ids[north_global_clump_ids_idx].insert(
                                                    north_local_result.global_clump_id(north_local_clump_id));
                                                global_clump_ids[north_global_clump_ids_idx].insert(
                                                    south_local_result.global_clump_id(south_local_clump_id));

                                                global_clump_ids[south_global_clump_ids_idx].insert(
                                                    north_local_result.global_clump_id(north_local_clump_id));
                                                global_clump_ids[south_global_clump_ids_idx].insert(
                                                    south_local_result.global_clump_id(south_local_clump_id));

                                                lue_hpx_assert(
                                                    std::find(
                                                        global_clump_ids[north_global_clump_ids_idx].begin(),
                                                        global_clump_ids[north_global_clump_ids_idx].end(),
                                                        north_local_result.global_clump_id(
                                                            north_local_clump_id)) !=
                                                    global_clump_ids[north_global_clump_ids_idx].end());
                                                lue_hpx_assert(
                                                    std::find(
                                                        global_clump_ids[south_global_clump_ids_idx].begin(),
                                                        global_clump_ids[south_global_clump_ids_idx].end(),
                                                        south_local_result.global_clump_id(
                                                            south_local_clump_id)) !=
                                                    global_clump_ids[south_global_clump_ids_idx].end());

                                                lue_hpx_assert(
                                                    global_clump_ids_by_local_clump_id.find(north_key) !=
                                                    global_clump_ids_by_local_clump_id.end());
                                                lue_hpx_assert(
                                                    global_clump_ids_by_local_clump_id.find(south_key) !=
                                                    global_clump_ids_by_local_clump_id.end());
                                            }

                                            clump_handled.insert(clump_data[north_idx]);
                                        }
                                    }
                                }
                            }
                        }

                        if (nr_cols_in_partitions > 1)
                        {
                            for (Index row_in_partitions = 0; row_in_partitions < nr_rows_in_partitions;
                                 ++row_in_partitions)
                            {
                                for (Index col_in_partitions = 0;
                                     col_in_partitions < nr_cols_in_partitions - 1;
                                     ++col_in_partitions)
                                {
                                    auto& west_local_result{
                                        local_results(row_in_partitions, col_in_partitions)};
                                    auto& east_local_result{
                                        local_results(row_in_partitions, col_in_partitions + 1)};

                                    // Yes, the east side of the west partition is the west side
                                    // of the array we build here, etc
                                    auto const& [west_zone_ids, west_clump_ids] =
                                        west_local_result.east_side();
                                    auto const& [east_zone_ids, east_clump_ids] =
                                        east_local_result.west_side();

                                    lue_hpx_assert(
                                        west_zone_ids.get().shape()[0] == east_zone_ids.get().shape()[0]);
                                    lue_hpx_assert(west_zone_ids.get().shape()[1] == 1);
                                    lue_hpx_assert(east_zone_ids.get().shape()[1] == 1);

                                    // To keep cells in the west and east columns consecutive
                                    // in memory, we "rotate" the sides: the upper row will contain
                                    // the west cells, the bottom row the east cells. A cell
                                    // at index idx in these rows correspond with a cell at column
                                    // idx in the original side array.

                                    ZoneData zone_data{{2, west_zone_ids.get().shape()[0]}};
                                    auto const [nr_rows, nr_cols] = zone_data.shape();

                                    std::copy(
                                        west_zone_ids.get().begin(),
                                        west_zone_ids.get().end(),
                                        zone_data.begin());
                                    std::copy(
                                        east_zone_ids.get().begin(),
                                        east_zone_ids.get().end(),
                                        zone_data.begin() + nr_cols);

                                    ClumpElement clump_nd{};
                                    ondp.mark_no_data(clump_nd);
                                    ClumpData clump_data{zone_data.shape(), clump_nd};

                                    {
                                        ClumpElement clump_id{0};

                                        for (Count row = 0; row < nr_rows; ++row)
                                        {
                                            for (Count col = 0; col < nr_cols; ++col)
                                            {
                                                // Skip cells that do not contain a valid zone ID, or that
                                                // already contain a valid clump ID
                                                if ((!indp.is_no_data(zone_data, row, col)) &&
                                                    ondp.is_no_data(clump_data, row, col))
                                                {
                                                    flood_fill(
                                                        indp,
                                                        ondp,
                                                        zone_data,
                                                        {row, col},
                                                        clump_data,
                                                        clump_id,
                                                        connectivity);
                                                    ++clump_id;
                                                }
                                            }
                                        }
                                    }


                                    // Find indxs of cells on both sides of the partition
                                    // boundary that belong to the same clump.
                                    // For each of those indxs, add information about the
                                    // partition_idx, local_clump_id, global_clump_id to a
                                    // collection, if not already present. Aggregate information
                                    // about the same clump.

                                    std::set<ClumpElement> clump_handled{};

                                    for (Index west_idx = 0; west_idx < nr_cols; ++west_idx)
                                    {
                                        if (!indp.is_no_data(clump_data[west_idx]) &&
                                            !clump_handled.contains(clump_data[west_idx]))
                                        {
                                            Index const east_idx = std::distance(
                                                clump_data.begin() + nr_cols,
                                                std::find(
                                                    clump_data.begin() + nr_cols,
                                                    clump_data.end(),
                                                    clump_data[west_idx]));

                                            if (east_idx != nr_cols)
                                            {
                                                // Clump extends on both sides of the partition boundary
                                                auto const west_local_clump_id{
                                                    west_clump_ids.get()[west_idx]};
                                                auto const east_local_clump_id{
                                                    east_clump_ids.get()[east_idx]};

                                                auto const west_key = std::make_tuple(
                                                    row_in_partitions,
                                                    col_in_partitions,
                                                    west_local_clump_id);
                                                auto const east_key = std::make_tuple(
                                                    row_in_partitions,
                                                    col_in_partitions + 1,
                                                    east_local_clump_id);

                                                auto west_it =
                                                    global_clump_ids_by_local_clump_id.find(west_key);
                                                auto east_it =
                                                    global_clump_ids_by_local_clump_id.find(east_key);

                                                Index west_global_clump_ids_idx{-1};
                                                Index east_global_clump_ids_idx{-1};

                                                if (west_it != global_clump_ids_by_local_clump_id.end())
                                                {
                                                    west_global_clump_ids_idx = west_it->second;

                                                    if (east_it == global_clump_ids_by_local_clump_id.end())
                                                    {
                                                        // west_it != end && east_it == end
                                                        east_global_clump_ids_idx = west_global_clump_ids_idx;
                                                        global_clump_ids_by_local_clump_id[east_key] =
                                                            east_global_clump_ids_idx;
                                                    }
                                                    else
                                                    {
                                                        east_global_clump_ids_idx = east_it->second;
                                                    }
                                                }
                                                else if (east_it != global_clump_ids_by_local_clump_id.end())
                                                {
                                                    // west_it == end && east_it != end
                                                    east_global_clump_ids_idx = east_it->second;
                                                    west_global_clump_ids_idx = east_global_clump_ids_idx;
                                                    global_clump_ids_by_local_clump_id[west_key] =
                                                        west_global_clump_ids_idx;
                                                }
                                                else
                                                {
                                                    // west_it == end && east_it == end
                                                    global_clump_ids.push_back({});
                                                    west_global_clump_ids_idx = global_clump_ids.size() - 1;
                                                    east_global_clump_ids_idx = west_global_clump_ids_idx;
                                                    global_clump_ids_by_local_clump_id[west_key] =
                                                        west_global_clump_ids_idx;
                                                    global_clump_ids_by_local_clump_id[east_key] =
                                                        east_global_clump_ids_idx;
                                                }

                                                lue_hpx_assert(west_global_clump_ids_idx >= 0);
                                                lue_hpx_assert(east_global_clump_ids_idx >= 0);

                                                global_clump_ids[west_global_clump_ids_idx].insert(
                                                    west_local_result.global_clump_id(west_local_clump_id));
                                                global_clump_ids[west_global_clump_ids_idx].insert(
                                                    east_local_result.global_clump_id(east_local_clump_id));

                                                global_clump_ids[east_global_clump_ids_idx].insert(
                                                    west_local_result.global_clump_id(west_local_clump_id));
                                                global_clump_ids[east_global_clump_ids_idx].insert(
                                                    east_local_result.global_clump_id(east_local_clump_id));

                                                lue_hpx_assert(
                                                    std::find(
                                                        global_clump_ids[west_global_clump_ids_idx].begin(),
                                                        global_clump_ids[west_global_clump_ids_idx].end(),
                                                        west_local_result.global_clump_id(
                                                            west_local_clump_id)) !=
                                                    global_clump_ids[west_global_clump_ids_idx].end());
                                                lue_hpx_assert(
                                                    std::find(
                                                        global_clump_ids[east_global_clump_ids_idx].begin(),
                                                        global_clump_ids[east_global_clump_ids_idx].end(),
                                                        east_local_result.global_clump_id(
                                                            east_local_clump_id)) !=
                                                    global_clump_ids[east_global_clump_ids_idx].end());

                                                lue_hpx_assert(
                                                    global_clump_ids_by_local_clump_id.find(west_key) !=
                                                    global_clump_ids_by_local_clump_id.end());
                                                lue_hpx_assert(
                                                    global_clump_ids_by_local_clump_id.find(east_key) !=
                                                    global_clump_ids_by_local_clump_id.end());
                                            }

                                            clump_handled.insert(clump_data[west_idx]);
                                        }
                                    }
                                }
                            }
                        }

                        if (connectivity == Connectivity::diagonal)
                        {
                            if (nr_rows_in_partitions > 1 && nr_cols_in_partitions > 1)
                            {
                                for (Index row_in_partitions = 0;
                                     row_in_partitions < nr_rows_in_partitions - 1;
                                     ++row_in_partitions)
                                {
                                    for (Index col_in_partitions = 0;
                                         col_in_partitions < nr_cols_in_partitions - 1;
                                         ++col_in_partitions)
                                    {
                                        auto& north_west_local_result{
                                            local_results(row_in_partitions, col_in_partitions)};
                                        auto& north_east_local_result{
                                            local_results(row_in_partitions, col_in_partitions + 1)};
                                        auto& south_west_local_result{
                                            local_results(row_in_partitions + 1, col_in_partitions)};
                                        auto& south_east_local_result{
                                            local_results(row_in_partitions + 1, col_in_partitions + 1)};

                                        auto const& [north_west_zone_id, north_west_local_clump_id] =
                                            north_west_local_result.south_east_corner();
                                        auto const& [north_east_zone_id, north_east_local_clump_id] =
                                            north_east_local_result.south_west_corner();
                                        auto const& [south_west_zone_id, south_west_local_clump_id] =
                                            south_west_local_result.north_east_corner();
                                        auto const& [south_east_zone_id, south_east_local_clump_id] =
                                            south_east_local_result.north_west_corner();

                                        ZoneData zone_data{{2, 2}};
                                        auto const [nr_rows, nr_cols] = zone_data.shape();

                                        Index const north_west_idx{0};
                                        Index const north_east_idx{1};
                                        Index const south_west_idx{2};
                                        Index const south_east_idx{3};

                                        zone_data[north_west_idx] = north_west_zone_id;
                                        zone_data[north_east_idx] = north_east_zone_id;
                                        zone_data[south_west_idx] = south_west_zone_id;
                                        zone_data[south_east_idx] = south_east_zone_id;

                                        ClumpElement clump_nd{};
                                        ondp.mark_no_data(clump_nd);
                                        ClumpData clump_data{zone_data.shape(), clump_nd};

                                        {
                                            ClumpElement clump_id{0};

                                            for (Count row = 0; row < nr_rows; ++row)
                                            {
                                                for (Count col = 0; col < nr_cols; ++col)
                                                {
                                                    // Skip cells that do not contain a valid zone ID, or
                                                    // that already contain a valid clump ID
                                                    if ((!indp.is_no_data(zone_data, row, col)) &&
                                                        ondp.is_no_data(clump_data, row, col))
                                                    {
                                                        flood_fill(
                                                            indp,
                                                            ondp,
                                                            zone_data,
                                                            {row, col},
                                                            clump_data,
                                                            clump_id,
                                                            connectivity);
                                                        ++clump_id;
                                                    }
                                                }
                                            }
                                        }

                                        // We only need to check whether clumps extend in diagonal
                                        // direction. Other direction is handled above already.

                                        if (!indp.is_no_data(clump_data[north_west_idx]) &&
                                            !indp.is_no_data(clump_data[south_east_idx]) &&
                                            clump_data[north_west_idx] == clump_data[south_east_idx])
                                        {
                                            // Clump extends in north-west / south-east direction
                                            auto const north_west_key = std::make_tuple(
                                                row_in_partitions,
                                                col_in_partitions,
                                                north_west_local_clump_id);
                                            auto const south_east_key = std::make_tuple(
                                                row_in_partitions + 1,
                                                col_in_partitions + 1,
                                                south_east_local_clump_id);

                                            auto north_west_it =
                                                global_clump_ids_by_local_clump_id.find(north_west_key);
                                            auto south_east_it =
                                                global_clump_ids_by_local_clump_id.find(south_east_key);

                                            Index north_west_global_clump_ids_idx{-1};
                                            Index south_east_global_clump_ids_idx{-1};

                                            if (north_west_it != global_clump_ids_by_local_clump_id.end())
                                            {
                                                north_west_global_clump_ids_idx = north_west_it->second;

                                                if (south_east_it == global_clump_ids_by_local_clump_id.end())
                                                {
                                                    // north_west_it != end && south_east_it == end
                                                    south_east_global_clump_ids_idx =
                                                        north_west_global_clump_ids_idx;
                                                    global_clump_ids_by_local_clump_id[south_east_key] =
                                                        south_east_global_clump_ids_idx;
                                                }
                                                else
                                                {
                                                    south_east_global_clump_ids_idx = south_east_it->second;
                                                }
                                            }
                                            else if (
                                                south_east_it != global_clump_ids_by_local_clump_id.end())
                                            {
                                                // north_west_it == end && south_east_it != end
                                                south_east_global_clump_ids_idx = south_east_it->second;
                                                north_west_global_clump_ids_idx =
                                                    south_east_global_clump_ids_idx;
                                                global_clump_ids_by_local_clump_id[north_west_key] =
                                                    north_west_global_clump_ids_idx;
                                            }
                                            else
                                            {
                                                // north_west_it == end && south_east_it == end
                                                global_clump_ids.push_back({});
                                                north_west_global_clump_ids_idx = global_clump_ids.size() - 1;
                                                south_east_global_clump_ids_idx =
                                                    north_west_global_clump_ids_idx;
                                                global_clump_ids_by_local_clump_id[north_west_key] =
                                                    north_west_global_clump_ids_idx;
                                                global_clump_ids_by_local_clump_id[south_east_key] =
                                                    south_east_global_clump_ids_idx;
                                            }

                                            lue_hpx_assert(north_west_global_clump_ids_idx >= 0);
                                            lue_hpx_assert(south_east_global_clump_ids_idx >= 0);

                                            global_clump_ids[north_west_global_clump_ids_idx].insert(
                                                north_west_local_result.global_clump_id(
                                                    north_west_local_clump_id));
                                            global_clump_ids[north_west_global_clump_ids_idx].insert(
                                                south_east_local_result.global_clump_id(
                                                    south_east_local_clump_id));

                                            global_clump_ids[south_east_global_clump_ids_idx].insert(
                                                north_west_local_result.global_clump_id(
                                                    north_west_local_clump_id));
                                            global_clump_ids[south_east_global_clump_ids_idx].insert(
                                                south_east_local_result.global_clump_id(
                                                    south_east_local_clump_id));

                                            lue_hpx_assert(
                                                std::find(
                                                    global_clump_ids[north_west_global_clump_ids_idx].begin(),
                                                    global_clump_ids[north_west_global_clump_ids_idx].end(),
                                                    north_west_local_result.global_clump_id(
                                                        north_west_local_clump_id)) !=
                                                global_clump_ids[north_west_global_clump_ids_idx].end());
                                            lue_hpx_assert(
                                                std::find(
                                                    global_clump_ids[south_east_global_clump_ids_idx].begin(),
                                                    global_clump_ids[south_east_global_clump_ids_idx].end(),
                                                    south_east_local_result.global_clump_id(
                                                        south_east_local_clump_id)) !=
                                                global_clump_ids[south_east_global_clump_ids_idx].end());

                                            lue_hpx_assert(
                                                global_clump_ids_by_local_clump_id.find(north_west_key) !=
                                                global_clump_ids_by_local_clump_id.end());
                                            lue_hpx_assert(
                                                global_clump_ids_by_local_clump_id.find(south_east_key) !=
                                                global_clump_ids_by_local_clump_id.end());
                                        }

                                        if (!indp.is_no_data(clump_data[north_east_idx]) &&
                                            !indp.is_no_data(clump_data[south_west_idx]) &&
                                            clump_data[north_east_idx] == clump_data[south_west_idx])
                                        {
                                            // Clump extends in north-west / south-east direction
                                            auto const north_east_key = std::make_tuple(
                                                row_in_partitions,
                                                col_in_partitions + 1,
                                                north_east_local_clump_id);
                                            auto const south_west_key = std::make_tuple(
                                                row_in_partitions + 1,
                                                col_in_partitions,
                                                south_west_local_clump_id);

                                            auto north_east_it =
                                                global_clump_ids_by_local_clump_id.find(north_east_key);
                                            auto south_west_it =
                                                global_clump_ids_by_local_clump_id.find(south_west_key);

                                            Index north_east_global_clump_ids_idx{-1};
                                            Index south_west_global_clump_ids_idx{-1};

                                            if (north_east_it != global_clump_ids_by_local_clump_id.end())
                                            {
                                                north_east_global_clump_ids_idx = north_east_it->second;

                                                if (south_west_it == global_clump_ids_by_local_clump_id.end())
                                                {
                                                    // north_east_it != end && south_west_it == end
                                                    south_west_global_clump_ids_idx =
                                                        north_east_global_clump_ids_idx;
                                                    global_clump_ids_by_local_clump_id[south_west_key] =
                                                        south_west_global_clump_ids_idx;
                                                }
                                                else
                                                {
                                                    south_west_global_clump_ids_idx = south_west_it->second;
                                                }
                                            }
                                            else if (
                                                south_west_it != global_clump_ids_by_local_clump_id.end())
                                            {
                                                // north_east_it == end && south_west_it != end
                                                south_west_global_clump_ids_idx = south_west_it->second;
                                                north_east_global_clump_ids_idx =
                                                    south_west_global_clump_ids_idx;
                                                global_clump_ids_by_local_clump_id[north_east_key] =
                                                    north_east_global_clump_ids_idx;
                                            }
                                            else
                                            {
                                                // north_east_it == end && south_west_it == end
                                                global_clump_ids.push_back({});
                                                north_east_global_clump_ids_idx = global_clump_ids.size() - 1;
                                                south_west_global_clump_ids_idx =
                                                    north_east_global_clump_ids_idx;
                                                global_clump_ids_by_local_clump_id[north_east_key] =
                                                    north_east_global_clump_ids_idx;
                                                global_clump_ids_by_local_clump_id[south_west_key] =
                                                    south_west_global_clump_ids_idx;
                                            }

                                            lue_hpx_assert(north_east_global_clump_ids_idx >= 0);
                                            lue_hpx_assert(south_west_global_clump_ids_idx >= 0);

                                            global_clump_ids[north_east_global_clump_ids_idx].insert(
                                                north_east_local_result.global_clump_id(
                                                    north_east_local_clump_id));
                                            global_clump_ids[north_east_global_clump_ids_idx].insert(
                                                south_west_local_result.global_clump_id(
                                                    south_west_local_clump_id));

                                            global_clump_ids[south_west_global_clump_ids_idx].insert(
                                                north_east_local_result.global_clump_id(
                                                    north_east_local_clump_id));
                                            global_clump_ids[south_west_global_clump_ids_idx].insert(
                                                south_west_local_result.global_clump_id(
                                                    south_west_local_clump_id));

                                            lue_hpx_assert(
                                                std::find(
                                                    global_clump_ids[north_east_global_clump_ids_idx].begin(),
                                                    global_clump_ids[north_east_global_clump_ids_idx].end(),
                                                    north_east_local_result.global_clump_id(
                                                        north_east_local_clump_id)) !=
                                                global_clump_ids[north_east_global_clump_ids_idx].end());
                                            lue_hpx_assert(
                                                std::find(
                                                    global_clump_ids[south_west_global_clump_ids_idx].begin(),
                                                    global_clump_ids[south_west_global_clump_ids_idx].end(),
                                                    south_west_local_result.global_clump_id(
                                                        south_west_local_clump_id)) !=
                                                global_clump_ids[south_west_global_clump_ids_idx].end());

                                            lue_hpx_assert(
                                                global_clump_ids_by_local_clump_id.find(north_east_key) !=
                                                global_clump_ids_by_local_clump_id.end());
                                            lue_hpx_assert(
                                                global_clump_ids_by_local_clump_id.find(south_west_key) !=
                                                global_clump_ids_by_local_clump_id.end());
                                        }
                                    }
                                }
                            }
                        }

                        // Update global clump IDs for those local IDs that must be merged. Use the
                        // lowest original global clump ID as the final global clump ID. Keep track
                        // of the global clump IDs that got removed this way. Sort this collection.

                        // Iterate over all (partition, local clump ID) tuples and replace the associated
                        // global clump ID with the lowest global clump ID of the current global clump IDs
                        // that must be merged.

                        std::set<ClumpElement> global_clump_ids_removed{};

                        for (auto const& [clump_tuple, global_clump_ids_idx] :
                             global_clump_ids_by_local_clump_id)
                        {
                            auto const [row, col, local_clump_id] = clump_tuple;
                            ClumpElement const new_global_clump_id =
                                *global_clump_ids[global_clump_ids_idx].begin();

                            local_results(row, col).update_global_clump_id(
                                local_clump_id, new_global_clump_id);

                            global_clump_ids_removed.insert(
                                ++global_clump_ids[global_clump_ids_idx].begin(),
                                global_clump_ids[global_clump_ids_idx].end());
                        }

                        // Given the sorted collection of global clump IDs that got removed,
                        // update the global IDs in all lookup tables to end up with a consecutive
                        // range of global clump IDs.
                        if (!global_clump_ids.empty())
                        {
                            for (Index row_in_partitions = 0; row_in_partitions < nr_rows_in_partitions;
                                 ++row_in_partitions)
                            {
                                for (Index col_in_partitions = 0; col_in_partitions < nr_cols_in_partitions;
                                     ++col_in_partitions)
                                {
                                    local_results(row_in_partitions, col_in_partitions)
                                        .update_global_clump_ids(global_clump_ids_removed);
                                }
                            }
                        }

                        return local_results;
                    }));
        }


        template<typename NoDataPolicy, typename Partition, typename LUT>
        auto reclass_partition(NoDataPolicy const& ndp, Partition const& clump_partition, LUT const& lut)
            -> Partition
        {
            lue_hpx_assert(clump_partition.is_ready());

            // Iterate over all cells containing a valid local clump ID and replace this value
            // with the associated global clump ID

            auto clump_data = ready_component_ptr(clump_partition)->data();
            auto const nr_elements = lue::nr_elements(clump_data.shape());

            for (Index idx = 0; idx < nr_elements; ++idx)
            {
                if (!ndp.is_no_data(clump_data, idx))
                {
                    clump_data[idx] = lut.find(clump_data[idx])->second;
                }
            }

            return clump_partition;
        }


        template<typename NoDataPolicy, typename Partition, typename LUT>
        struct ReclassPartitionAction:
            hpx::actions::make_action<
                decltype(&reclass_partition<NoDataPolicy, Partition, LUT>),
                &reclass_partition<NoDataPolicy, Partition, LUT>,
                ReclassPartitionAction<NoDataPolicy, Partition, LUT>>::type
        {
        };


        template<typename Policies, typename Partitions>
        auto solve_clump_globally(
            Policies const& policies,
            ShapeT<Partitions> const& shape_in_partitions,
            LocalResultsF<PartitionT<Partitions>>&& local_results_f) -> Partitions
        {
            using Partition = PartitionT<Partitions>;

            Partitions clump_partitions{shape_in_partitions};
            detail::IDPromiseArray<rank<Partitions>> clump_partition_promises{shape_in_partitions};
            Count const nr_partitions{nr_elements(shape_in_partitions)};

            for (Index partition_idx = 0; partition_idx < nr_partitions; ++partition_idx)
            {
                clump_partitions[partition_idx] =
                    Partition{clump_partition_promises[partition_idx].get_future()};
            }

            using ClumpPartition = PartitionT<Partitions>;
            using LUT = typename LocalResult<ClumpPartition>::LUT;

            local_results_f.then(hpx::unwrapping(
                [policies, clump_partition_promises = std::move(clump_partition_promises), nr_partitions](
                    auto&& local_results) mutable
                {
                    auto const& ondp = std::get<0>(policies.outputs_policies()).output_no_data_policy();

                    // Iterate over all partitions and asynchronously spawn a task which reclassifies the
                    // local clump IDs into global clump IDs

                    ReclassPartitionAction<decltype(ondp), ClumpPartition, LUT> action{};

                    for (Index partition_idx = 0; partition_idx < nr_partitions; ++partition_idx)
                    {
                        auto& local_result{local_results[partition_idx]};
                        auto partition{local_result.partition()};
                        auto lut{local_result.lut()};

                        lue_hpx_assert(partition.is_ready());

                        clump_partition_promises[partition_idx].set_value(
                            Partition{hpx::async(
                                          action,
                                          hpx::get_colocation_id(hpx::launch::sync, partition.get_id()),
                                          ondp,
                                          std::move(partition),
                                          std::move(lut))}
                                .get_id());
                    }
                }));

            return clump_partitions;
        }

    }  // namespace detail::clump


    template<typename Policies, Rank rank>
    auto clump(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& zone,
        Connectivity const connectivity) -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank>
    {
        static_assert(
            std::is_same_v<policy::InputElementT<Policies, 0>, policy::OutputElementT<Policies, 0>>);
        static_assert(rank == 2);

        using ClumpArray = PartitionedArray<policy::OutputElementT<Policies, 0>, rank>;
        using ClumpPartitions = PartitionsT<ClumpArray>;

        auto local_result_fs = detail::clump::solve_clump_locally(policies, zone, connectivity);
        auto local_results_f =
            detail::clump::determine_reclass_tables(policies, std::move(local_result_fs), connectivity);
        auto clump_partitions = detail::clump::solve_clump_globally<Policies, ClumpPartitions>(
            policies, zone.partitions().shape(), std::move(local_results_f));

        return {shape(zone), zone.localities(), std::move(clump_partitions)};
    }

}  // namespace lue


#define LUE_INSTANTIATE_CLUMP(Policies, rank)                                                                \
                                                                                                             \
    template LUE_ZONAL_OPERATION_EXPORT PartitionedArray<policy::OutputElementT<Policies, 0>, rank>          \
    clump<ArgumentType<void(Policies)>, rank>(                                                               \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<policy::InputElementT<Policies, 0>, rank> const&,                                   \
        Connectivity connectivity);
