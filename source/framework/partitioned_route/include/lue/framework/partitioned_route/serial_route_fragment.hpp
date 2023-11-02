#pragma once
#include "lue/framework/core/assert.hpp"
#include "lue/framework/core/indices.hpp"
#include <hpx/runtime_distributed.hpp>
#include <algorithm>
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
            /// using Location = std::tuple<PartitionID, Locality>;
            /// TODO Turn PartitionID into a hpx::shared_future<hpx::id_type> and the shared_future back into
            /// a Locality
            using Location = std::tuple<PartitionID, hpx::shared_future<hpx::id_type>>;

            //! Linear index of a cell, within a partition
            /// using CellIdxs = Indices<Index, rank>;
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


            bool operator==(SerialRouteFragment const& other) const
            {
                // Test for same size first, otherwise the value of mismatch.first can be end()
                // even if a mismatch was found (if other.idxs contains more elements).
                bool const same_size{std::size(_idxs) == std::size(other._idxs)};
                bool const same_next_fragment_location{
                    _next_fragment_location == other._next_fragment_location};
                bool result{false};

                if (same_size && same_next_fragment_location)
                {
                    // Only now call mismatch. Collections are equal if no mismatch was found
                    result =
                        std::mismatch(_idxs.begin(), _idxs.end(), other._idxs.begin()).first == _idxs.end();
                }

                return result;
            }


            bool operator!=(SerialRouteFragment const& other) const
            {
                return !(*this == other);
            }


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

                _next_fragment_location = location;
            }


            CellIdxs const& cell_idxs() const
            {
                return _idxs;
            }


            std::optional<Location> const& next_fragment_location() const
            {
                return _next_fragment_location;
            }


        private:

            //! All cell indices that are part of the fragment
            CellIdxs _idxs;

            //! Location of the next fragment, if any
            std::optional<Location> _next_fragment_location;
    };

}  // namespace lue
