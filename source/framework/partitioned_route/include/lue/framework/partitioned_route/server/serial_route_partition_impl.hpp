#pragma once
#include "lue/framework/partitioned_route/server/serial_route_partition.hpp"


namespace lue::server {

    template<Rank rank>
    SerialRoutePartition<rank>::SerialRoutePartition():

        Base{},
        _offset{},
        _shape{},
        _route_fragments{}

    {
        assert_invariants();
    }


    template<Rank rank>
    SerialRoutePartition<rank>::SerialRoutePartition(Offset const& offset, Shape const& shape):

        SerialRoutePartition{offset, shape, {}}

    {
        assert_invariants();
    }


    template<Rank rank>
    SerialRoutePartition<rank>::SerialRoutePartition(
        Offset const& offset, Shape const& shape, RouteFragments&& route_fragments):

        Base{},
        _offset{offset},
        _shape{shape},
        _route_fragments{std::move(route_fragments)}

    {
        assert_invariants();
    }


    template<Rank rank>
    typename SerialRoutePartition<rank>::Offset SerialRoutePartition<rank>::offset() const
    {
        return _offset;
    }


    template<Rank rank>
    typename SerialRoutePartition<rank>::Shape SerialRoutePartition<rank>::shape() const
    {
        return _shape;
    }


    template<Rank rank>
    Count SerialRoutePartition<rank>::nr_routes() const
    {
        return std::size(_route_fragments);
    }


    template<Rank rank>
    Count SerialRoutePartition<rank>::nr_route_fragments() const
    {
        Count result{0};

        for (auto const& [route_id, fragments] : _route_fragments)
        {
            result += std::size(fragments);
        }

        return result;
    }


    template<Rank rank>
    std::set<typename SerialRoutePartition<rank>::RouteID> SerialRoutePartition<rank>::route_ids() const
    {
        std::set<RouteID> result{};

        for (auto const& [route_id, fragments] : _route_fragments)
        {
            result.insert(route_id);
        }

        return result;
    }


    template<Rank rank>
    std::vector<typename SerialRoutePartition<rank>::RouteFragment>
    SerialRoutePartition<rank>::route_fragments(RouteID const route_id) const
    {
        auto const it = _route_fragments.find(route_id);

        lue_hpx_assert(it != _route_fragments.end());

        return it->second;
    }


    template<Rank rank>
    std::vector<typename SerialRoutePartition<rank>::RouteFragmentLocation>
    SerialRoutePartition<rank>::remote_route_fragment_locations() const
    {
        std::vector<RouteFragmentLocation> locations{};

        for (auto const& [route_id, fragments] : _route_fragments)
        {
            for (auto const& fragment : fragments)
            {
                if (!fragment.is_last())
                {
                    locations.push_back(fragment.next_fragment_location());
                }
            }
        }

        return locations;
    }


    template<Rank rank>
    void SerialRoutePartition<rank>::set_route_fragments(RouteFragments&& route_fragments)
    {
        _route_fragments = std::move(route_fragments);
    }


    template<Rank rank>
    void SerialRoutePartition<rank>::assert_invariants() const
    {
        // Verify all cells idxs in the fragments are within the partition
        for (auto const& [route_id, fragments] : _route_fragments)
        {
            for (auto const& fragment : fragments)
            {
                for ([[maybe_unused]] auto const& cell_idx : fragment.cell_idxs())
                {
                    lue_hpx_assert(cell_idx >= 0);
                    lue_hpx_assert(cell_idx < nr_elements(_shape));
                }
            }
        }
    }

}  // namespace lue::server
