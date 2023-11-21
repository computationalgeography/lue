#pragma once
#include "lue/framework/core/assert.hpp"
#include "lue/framework/partitioned_route/serial_route_partition.hpp"


namespace lue {

    template<Rank rank>
    SerialRoutePartition<rank>::SerialRoutePartition():

        Base{}

    {
    }


    template<Rank rank>
    SerialRoutePartition<rank>::SerialRoutePartition(hpx::id_type component_id):

        Base{component_id}

    {
    }


    template<Rank rank>
    SerialRoutePartition<rank>::SerialRoutePartition(hpx::future<hpx::id_type>&& component_id):

        Base{std::move(component_id)}

    {
    }


    template<Rank rank>
    SerialRoutePartition<rank>::SerialRoutePartition(hpx::shared_future<hpx::id_type>&& component_id):

        Base{std::move(component_id)}

    {
    }


    template<Rank rank>
    SerialRoutePartition<rank>::SerialRoutePartition(hpx::future<SerialRoutePartition>&& partition):

        Base{std::move(partition)}

    {
    }


    template<Rank rank>
    SerialRoutePartition<rank>::SerialRoutePartition(
        hpx::id_type const locality_id, Offset const& offset, Shape const& shape):

        Base{hpx::new_<Server>(locality_id, offset, shape)}

    {
    }


    template<Rank rank>
    hpx::future<typename SerialRoutePartition<rank>::Offset> SerialRoutePartition<rank>::offset() const
    {
        lue_hpx_assert(this->is_ready());
        lue_hpx_assert(this->get_id());

        typename Server::OffsetAction action;

        // this->get_id() identifies the server instance
        return hpx::async(action, this->get_id());
    }


    template<Rank rank>
    hpx::future<typename SerialRoutePartition<rank>::Shape> SerialRoutePartition<rank>::shape() const
    {
        lue_hpx_assert(this->is_ready());
        lue_hpx_assert(this->get_id());

        typename Server::ShapeAction action;

        // this->get_id() identifies the server instance
        return hpx::async(action, this->get_id());
    }


    template<Rank rank>
    hpx::future<Count> SerialRoutePartition<rank>::nr_routes() const
    {
        lue_hpx_assert(this->is_ready());
        lue_hpx_assert(this->get_id());

        typename Server::NrRoutesAction action;

        return hpx::async(action, this->get_id());
    }


    template<Rank rank>
    hpx::future<Count> SerialRoutePartition<rank>::nr_route_fragments() const
    {
        lue_hpx_assert(this->is_ready());
        lue_hpx_assert(this->get_id());

        typename Server::NrRouteFragmentsAction action;

        return hpx::async(action, this->get_id());
    }


    template<Rank rank>
    hpx::future<std::set<typename SerialRoutePartition<rank>::RouteID>>
    SerialRoutePartition<rank>::route_ids() const
    {
        lue_hpx_assert(this->is_ready());
        lue_hpx_assert(this->get_id());

        typename Server::RouteIDsAction action;

        return hpx::async(action, this->get_id());
    }


    template<Rank rank>
    hpx::future<std::vector<typename SerialRoutePartition<rank>::RouteFragment>>
    SerialRoutePartition<rank>::route_fragments(RouteID const route_id) const
    {
        lue_hpx_assert(this->is_ready());
        lue_hpx_assert(this->get_id());

        typename Server::RouteFragmentsAction action;

        return hpx::async(action, this->get_id(), route_id);
    }


    template<Rank rank>
    hpx::future<std::vector<typename SerialRoutePartition<rank>::RouteFragmentLocation>>
    SerialRoutePartition<rank>::remote_route_fragment_locations() const
    {
        lue_hpx_assert(this->is_ready());
        lue_hpx_assert(this->get_id());

        typename Server::RemoteRouteFragmentLocationsAction action;

        return hpx::async(action, this->get_id());
    }

}  // namespace lue
