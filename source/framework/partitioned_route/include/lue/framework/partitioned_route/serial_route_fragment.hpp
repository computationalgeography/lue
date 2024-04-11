#pragma once
#include "lue/framework/core/assert.hpp"
#include "lue/framework/core/indices.hpp"
#include "lue/framework/partitioned_route/export.hpp"
#include <hpx/runtime_distributed.hpp>
#include <algorithm>
#include <optional>
#include <tuple>
#include <vector>


namespace lue {

    // template<Rank rank>
    // class SerialRoutePartition;


    /*!
        @brief      Class for aggregating information about a fragment of a serial route

        A serial route is a collection of route fragments scattered over route partitions. A
        fragment is a piece of a route whose cell indxs are all located in a single partition.
    */
    template<Rank rank>
    class LUE_FPR_EXPORT SerialRouteFragment
    {
        public:

            //! ID of a route partition
            using PartitionID = hpx::id_type;

            //! Locality a partition is located in
            using LocalityID = hpx::id_type;

            //! Location of a route fragment
            using Location = hpx::shared_future<PartitionID>;

            //! Linear index of a cell, within a partition
            using CellIdx = Index;

            using CellIdxs = std::vector<CellIdx>;


            SerialRouteFragment() = default;

            SerialRouteFragment(SerialRouteFragment const&) = default;

            SerialRouteFragment(SerialRouteFragment&&) = default;

            SerialRouteFragment(CellIdxs const& idxs):

                _idxs{idxs},
                _next_fragment_location{}

            {
            }


            SerialRouteFragment(CellIdxs&& idxs):

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


            void append(CellIdx const& cell_idxs)
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
                lue_hpx_assert(location.valid());

                _next_fragment_location = location;
            }


            CellIdxs const& cell_idxs() const
            {
                return _idxs;
            }


            Location const& next_fragment_location() const
            {
                lue_hpx_assert(!is_last());
                lue_hpx_assert(_next_fragment_location.value().valid());

                return _next_fragment_location.value();
            }


            bool is_last() const
            {
                return !_next_fragment_location.has_value();
            }


        private:

            //! All cell indices that are part of the fragment
            CellIdxs _idxs;

            //! Location of the next fragment, if any
            std::optional<Location> _next_fragment_location;
    };

}  // namespace lue
