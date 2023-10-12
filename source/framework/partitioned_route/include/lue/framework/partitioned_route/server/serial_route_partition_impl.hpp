#pragma once
#include "lue/framework/partitioned_route/server/serial_route_partition.hpp"


namespace lue::server {

    template<Rank rank>
    SerialRoutePartition<rank>::SerialRoutePartition():

        Base{},
        _shape{},
        _route_fragments{}

    {
        assert_invariants();
    }


    template<Rank rank>
    SerialRoutePartition<rank>::SerialRoutePartition(Shape const& shape, RouteFragments&& route_fragments):

        Base{},
        _shape{shape},
        _route_fragments{std::move(route_fragments)}

    {
        assert_invariants();
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
    std::vector<typename SerialRoutePartition<rank>::RouteID> SerialRoutePartition<rank>::route_ids() const
    {
        std::vector<RouteID> result{};
        result.reserve(std::size(_route_fragments));

        for (auto const& [route_id, fragments] : _route_fragments)
        {
            result.push_back(route_id);
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
    void SerialRoutePartition<rank>::assert_invariants() const
    {
        // Verify all cells idxs in the fragments are within the partition
        for (auto const& [route_id, fragments] : _route_fragments)
        {
            for (auto const& fragment : fragments)
            {
                for (auto const& cell_idxs : fragment.cells_idxs())
                {
                    for (Index idx = 0; idx < rank; ++idx)
                    {
                        lue_hpx_assert(cell_idxs[idx] >= 0);
                        lue_hpx_assert(cell_idxs[idx] < _shape[idx]);
                    }
                }
            }
        }
    }

}  // namespace lue::server
