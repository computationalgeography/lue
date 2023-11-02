#pragma once
#include "lue/framework/partitioned_route/serial_route_fragment.hpp"
#include <hpx/datastructures/serialization/optional.hpp>
#include <hpx/serialization/map.hpp>


namespace hpx::serialization {

    template<lue::Rank rank>
    void serialize(
        input_archive& archive,
        lue::SerialRouteFragment<rank>& fragment,
        [[maybe_unused]] unsigned int const version)
    {
        // Read fragment from archive
        using Fragment = lue::SerialRouteFragment<rank>;
        using CellIdxs = typename Fragment::CellIdxs;
        using Location = typename Fragment::Location;

        CellIdxs cell_idxs{};
        std::optional<Location> next_fragment_location{};

        archive& cell_idxs& next_fragment_location;

        fragment = Fragment{cell_idxs};

        if (next_fragment_location)
        {
            fragment.end(*next_fragment_location);
        }
    }


    template<lue::Rank rank>
    void serialize(
        output_archive& archive,
        lue::SerialRouteFragment<rank> const& fragment,
        [[maybe_unused]] unsigned int const version)
    {
        // Write buffer to archive
        archive& fragment.cell_idxs() & fragment.next_fragment_location();
    }

}  // namespace hpx::serialization
