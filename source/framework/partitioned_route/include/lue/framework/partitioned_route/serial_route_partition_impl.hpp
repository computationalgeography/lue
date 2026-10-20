#pragma once
#include "lue/framework/core/assert.hpp"
#include "lue/framework/partitioned_route/serial_route_partition_decl.hpp"


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
    SerialRoutePartition<RouteID, rank>::SerialRoutePartition(
        hpx::id_type locality_id, Offset const& offset, Shape const& shape, RouteFragments&& route_fragments):

        Base{hpx::new_<Server>(locality_id, offset, shape, std::move(route_fragments))}

    {
    }


    template<typename RouteID, Rank rank>
    auto SerialRoutePartition<RouteID, rank>::offset(hpx::launch::async_policy) const -> hpx::future<Offset>
    {
        lue_hpx_assert(this->is_ready());
        lue_hpx_assert(this->get_id());

        using Action = Server::OffsetAction;

        return hpx::async(Action{}, this->get_id());
    }


    template<typename RouteID, Rank rank>
    auto SerialRoutePartition<RouteID, rank>::offset(hpx::launch::sync_policy) const -> Offset
    {
        lue_hpx_assert(this->is_ready());
        lue_hpx_assert(this->get_id());

        using Action = Server::OffsetAction;

        return Action{}(this->get_id());
    }


    template<typename RouteID, Rank rank>
    auto SerialRoutePartition<RouteID, rank>::shape(hpx::launch::async_policy) const -> hpx::future<Shape>
    {
        lue_hpx_assert(this->is_ready());
        lue_hpx_assert(this->get_id());

        using Action = Server::ShapeAction;

        return hpx::async(Action{}, this->get_id());
    }


    template<typename RouteID, Rank rank>
    auto SerialRoutePartition<RouteID, rank>::shape(hpx::launch::sync_policy) const -> Shape
    {
        lue_hpx_assert(this->is_ready());
        lue_hpx_assert(this->get_id());

        using Action = Server::ShapeAction;

        return Action{}(this->get_id());
    }


    template<typename RouteID, Rank rank>
    auto SerialRoutePartition<RouteID, rank>::nr_routes(hpx::launch::async_policy) const -> hpx::future<Count>
    {
        lue_hpx_assert(this->is_ready());
        lue_hpx_assert(this->get_id());

        using Action = Server::NrRoutesAction;

        return hpx::async(Action{}, this->get_id());
    }


    template<typename RouteID, Rank rank>
    auto SerialRoutePartition<RouteID, rank>::nr_routes(hpx::launch::sync_policy) const -> Count
    {
        lue_hpx_assert(this->is_ready());
        lue_hpx_assert(this->get_id());

        using Action = Server::NrRoutesAction;

        return Action{}(this->get_id());
    }


    template<typename RouteID, Rank rank>
    auto SerialRoutePartition<RouteID, rank>::nr_route_fragments(hpx::launch::async_policy) const
        -> hpx::future<Count>
    {
        lue_hpx_assert(this->is_ready());
        lue_hpx_assert(this->get_id());

        using Action = Server::NrRouteFragmentsAction;

        return hpx::async(Action{}, this->get_id());
    }


    template<typename RouteID, Rank rank>
    auto SerialRoutePartition<RouteID, rank>::nr_route_fragments(hpx::launch::sync_policy) const -> Count
    {
        lue_hpx_assert(this->is_ready());
        lue_hpx_assert(this->get_id());

        using Action = Server::NrRouteFragmentsAction;

        return Action{}(this->get_id());
    }


    template<typename RouteID, Rank rank>
    auto SerialRoutePartition<RouteID, rank>::route_ids(hpx::launch::async_policy) const
        -> hpx::future<std::set<RouteID>>
    {
        lue_hpx_assert(this->is_ready());
        lue_hpx_assert(this->get_id());

        using Action = Server::RouteIDsAction;

        return hpx::async(Action{}, this->get_id());
    }


    template<typename RouteID, Rank rank>
    auto SerialRoutePartition<RouteID, rank>::route_ids(hpx::launch::sync_policy) const -> std::set<RouteID>
    {
        lue_hpx_assert(this->is_ready());
        lue_hpx_assert(this->get_id());

        using Action = Server::RouteIDsAction;

        return Action{}(this->get_id());
    }


    template<typename RouteID, Rank rank>
    auto SerialRoutePartition<RouteID, rank>::route_fragments(
        hpx::launch::async_policy, RouteID const route_id) const -> hpx::future<std::vector<RouteFragment>>
    {
        lue_hpx_assert(this->is_ready());
        lue_hpx_assert(this->get_id());

        using Action = Server::RouteFragmentsAction;

        return hpx::async(Action{}, this->get_id(), route_id);
    }


    template<typename RouteID, Rank rank>
    auto SerialRoutePartition<RouteID, rank>::route_fragments(
        hpx::launch::sync_policy, RouteID const route_id) const -> std::vector<RouteFragment>
    {
        lue_hpx_assert(this->is_ready());
        lue_hpx_assert(this->get_id());

        using Action = Server::RouteFragmentsAction;

        return Action{}(this->get_id(), route_id);
    }


    template<typename RouteID, Rank rank>
    auto SerialRoutePartition<RouteID, rank>::remote_route_fragment_locations(hpx::launch::async_policy) const
        -> hpx::future<std::vector<RouteFragmentLocation>>
    {
        lue_hpx_assert(this->is_ready());
        lue_hpx_assert(this->get_id());

        using Action = Server::RemoteRouteFragmentLocationsAction;

        return hpx::async(Action{}, this->get_id());
    }


    template<typename RouteID, Rank rank>
    auto SerialRoutePartition<RouteID, rank>::remote_route_fragment_locations(hpx::launch::sync_policy) const
        -> std::vector<RouteFragmentLocation>
    {
        lue_hpx_assert(this->is_ready());
        lue_hpx_assert(this->get_id());

        using Action = Server::RemoteRouteFragmentLocationsAction;

        return Action{}(this->get_id());
    }


    template<typename RouteID, Rank rank>
    auto SerialRoutePartition<RouteID, rank>::set_route_fragments(
        hpx::launch::async_policy, RouteFragments&& route_fragments) -> hpx::future<void>
    {
        lue_hpx_assert(this->is_ready());
        lue_hpx_assert(this->get_id());

        using Action = Server::SetRouteFragmentsAction;

        return hpx::async(Action{}, this->get_id(), std::move(route_fragments));
    }


    template<typename RouteID, Rank rank>
    auto SerialRoutePartition<RouteID, rank>::set_route_fragments(
        hpx::launch::sync_policy, RouteFragments&& route_fragments) -> void
    {
        lue_hpx_assert(this->is_ready());
        lue_hpx_assert(this->get_id());

        using Action = Server::SetRouteFragmentsAction;

        return Action{}(this->get_id(), std::move(route_fragments));
    }

}  // namespace lue
