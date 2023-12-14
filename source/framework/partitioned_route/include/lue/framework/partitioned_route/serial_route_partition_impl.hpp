#pragma once
#include "lue/framework/core/assert.hpp"
#include "lue/framework/partitioned_route/serial_route_partition.hpp"


namespace lue {

    template<typename RouteID, Rank rank>
    SerialRoutePartition<RouteID, rank>::SerialRoutePartition():

        Base{}

    {
    }


    template<typename RouteID, Rank rank>
    SerialRoutePartition<RouteID, rank>::SerialRoutePartition(hpx::id_type component_id):

        Base{component_id}

    {
    }


    template<typename RouteID, Rank rank>
    SerialRoutePartition<RouteID, rank>::SerialRoutePartition(hpx::future<hpx::id_type>&& component_id):

        Base{std::move(component_id)}

    {
    }


    template<typename RouteID, Rank rank>
    SerialRoutePartition<RouteID, rank>::SerialRoutePartition(
        hpx::shared_future<hpx::id_type>&& component_id):

        Base{std::move(component_id)}

    {
    }


    template<typename RouteID, Rank rank>
    SerialRoutePartition<RouteID, rank>::SerialRoutePartition(hpx::future<SerialRoutePartition>&& partition):

        Base{std::move(partition)}

    {
    }


    template<typename RouteID, Rank rank>
    SerialRoutePartition<RouteID, rank>::SerialRoutePartition(
        hpx::id_type const locality_id, Offset const& offset, Shape const& shape):

        Base{hpx::new_<Server>(locality_id, offset, shape)}

    {
    }


    template<typename RouteID, Rank rank>
    auto SerialRoutePartition<RouteID, rank>::offset() const -> hpx::future<Offset>
    {
        lue_hpx_assert(this->is_ready());
        lue_hpx_assert(this->get_id());

        typename Server::OffsetAction action;

        // this->get_id() identifies the server instance
        return hpx::async(action, this->get_id());
    }


    template<typename RouteID, Rank rank>
    auto SerialRoutePartition<RouteID, rank>::shape() const -> hpx::future<Shape>
    {
        lue_hpx_assert(this->is_ready());
        lue_hpx_assert(this->get_id());

        typename Server::ShapeAction action;

        // this->get_id() identifies the server instance
        return hpx::async(action, this->get_id());
    }


    template<typename RouteID, Rank rank>
    hpx::future<Count> SerialRoutePartition<RouteID, rank>::nr_routes() const
    {
        lue_hpx_assert(this->is_ready());
        lue_hpx_assert(this->get_id());

        typename Server::NrRoutesAction action;

        return hpx::async(action, this->get_id());
    }


    template<typename RouteID, Rank rank>
    hpx::future<Count> SerialRoutePartition<RouteID, rank>::nr_route_fragments() const
    {
        lue_hpx_assert(this->is_ready());
        lue_hpx_assert(this->get_id());

        typename Server::NrRouteFragmentsAction action;

        return hpx::async(action, this->get_id());
    }


    template<typename RouteID, Rank rank>
    hpx::future<std::set<RouteID>> SerialRoutePartition<RouteID, rank>::route_ids() const
    {
        lue_hpx_assert(this->is_ready());
        lue_hpx_assert(this->get_id());

        typename Server::RouteIDsAction action;

        return hpx::async(action, this->get_id());
    }


    template<typename RouteID, Rank rank>
    auto SerialRoutePartition<RouteID, rank>::route_fragments(RouteID const route_id) const
        -> hpx::future<std::vector<RouteFragment>>
    {
        lue_hpx_assert(this->is_ready());
        lue_hpx_assert(this->get_id());

        typename Server::RouteFragmentsAction action;

        return hpx::async(action, this->get_id(), route_id);
    }


    template<typename RouteID, Rank rank>
    auto SerialRoutePartition<RouteID, rank>::remote_route_fragment_locations() const
        -> hpx::future<std::vector<RouteFragmentLocation>>
    {
        lue_hpx_assert(this->is_ready());
        lue_hpx_assert(this->get_id());

        typename Server::RemoteRouteFragmentLocationsAction action;

        return hpx::async(action, this->get_id());
    }

}  // namespace lue
