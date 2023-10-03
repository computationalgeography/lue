#pragma once
#include "lue/framework/partitioned_route/server/serial_route_partition.hpp"


namespace lue::server {

    template<Rank rank>
    SerialRoutePartition<rank>::SerialRoutePartition():

        Base{},
        _route_fragments{}

    {
    }


    template<Rank rank>
    SerialRoutePartition<rank>::SerialRoutePartition(RouteFragments&& route_fragments):

        Base{},
        _route_fragments{std::move(route_fragments)}

    {
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

}  // namespace lue::server
