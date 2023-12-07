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
        bool is_last{false};

        // clang-format off
        archive & cell_idxs & is_last;

        fragment = Fragment{cell_idxs};

        if(!is_last)
        {
            Location next_fragment_location{};

            archive & next_fragment_location;

            lue_hpx_assert(next_fragment_location.valid());

            fragment.end(next_fragment_location);
        }
        // clang-format on
    }


    template<lue::Rank rank>
    void serialize(
        output_archive& archive,
        lue::SerialRouteFragment<rank> const& fragment,
        [[maybe_unused]] unsigned int const version)
    {
        // Write fragment to archive

        // clang-format off
        archive & fragment.cell_idxs();

        if(fragment.is_last())
        {
            archive & true;
        }
        else
        {
            lue_hpx_assert(fragment.next_fragment_location().valid());

            archive & false & fragment.next_fragment_location();
        }
        // clang-format on
    }

}  // namespace hpx::serialization
