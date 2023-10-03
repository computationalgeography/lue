#pragma once
#include "lue/framework/core/assert.hpp"
#include "lue/framework/core/indices.hpp"
#include <hpx/runtime_distributed.hpp>
#include <optional>
#include <tuple>
#include <vector>


namespace lue {

    /*!
        @brief      Class for aggregating information about a fragment of a serial route

        A serial route is a collection of route fragments scattered over route partitions. A
        fragment is a piece of a route whose cell indxs are all located in a single partition.
    */
    template<Rank rank>
    class SerialRouteFragment
    {
        public:

            //! ID of a route partition
            using PartitionID = hpx::id_type;

            //! Locality a partition is located in
            using Locality = hpx::id_type;

            //! Location of a route fragment
            using Location = std::tuple<PartitionID, Locality>;

            //! Indices of a cell, within a partition
            using CellIdxs = Indices<Index, rank>;

            using CellsIdxs = std::vector<CellIdxs>;


            SerialRouteFragment() = default;

            SerialRouteFragment(SerialRouteFragment const&) = default;

            SerialRouteFragment(SerialRouteFragment&&) = default;

            SerialRouteFragment(CellsIdxs const& idxs):

                _idxs{idxs},
                _next_fragment_location{}

            {
            }


            SerialRouteFragment(CellsIdxs&& idxs):

                _idxs{std::move(idxs)},
                _next_fragment_location{}

            {
            }


            ~SerialRouteFragment() = default;

            SerialRouteFragment& operator=(SerialRouteFragment const&) = default;

            SerialRouteFragment& operator=(SerialRouteFragment&&) = default;


            Count nr_cells() const
            {
                return std::size(_idxs);
            }


            void append(CellIdxs const& cell_idxs)
            {
                // It doesn't make sense to keep adding idxs when the location of the next
                // fragment is already known, does it?
                lue_hpx_assert(!_next_fragment_location);

                _idxs.push_back(cell_idxs);
            }


            void end(Location const& location)
            {
                // It doesn't make sense to change the location of the next fragment to something
                // else, does it?
                lue_hpx_assert(!_next_fragment_location);

                _next_fragment_location = location;
            }


            CellsIdxs const& cells_idxs() const
            {
                return _idxs;
            }


            std::optional<Location> const& next_fragment_location() const
            {
                return _next_fragment_location;
            }


        private:

            //! All cell indices that are part of the fragment
            CellsIdxs _idxs;

            //! Location of the next fragment, if any
            std::optional<Location> _next_fragment_location;
    };

}  // namespace lue
