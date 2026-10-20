#pragma once
#include "lue/framework/partitioned_route/server/serial_route_partition_decl.hpp"


namespace lue::server {

    template<typename RouteID, Rank rank>
    SerialRoutePartition<RouteID, rank>::SerialRoutePartition():

        Base{},
        _offset{},
        _shape{},
        _route_fragments{}

    {
        assert_invariants();
    }


    template<typename RouteID, Rank rank>
    SerialRoutePartition<RouteID, rank>::SerialRoutePartition(Offset const& offset, Shape const& shape):

        SerialRoutePartition{offset, shape, {}}

    {
        assert_invariants();
    }


    template<typename RouteID, Rank rank>
    SerialRoutePartition<RouteID, rank>::SerialRoutePartition(
        Offset const& offset, Shape const& shape, RouteFragments&& route_fragments):

        Base{},
        _offset{offset},
        _shape{shape},
        _route_fragments{std::move(route_fragments)}

    {
        assert_invariants();
    }


    template<typename RouteID, Rank rank>
    auto SerialRoutePartition<RouteID, rank>::offset() const -> Offset
    {
        return _offset;
    }


    template<typename RouteID, Rank rank>
    auto SerialRoutePartition<RouteID, rank>::shape() const -> Shape
    {
        return _shape;
    }


    template<typename RouteID, Rank rank>
    Count SerialRoutePartition<RouteID, rank>::nr_routes() const
    {
        return std::size(_route_fragments);
    }


    template<typename RouteID, Rank rank>
    Count SerialRoutePartition<RouteID, rank>::nr_route_fragments() const
    {
        Count result{0};

        for (auto const& [route_id, fragments] : _route_fragments)
        {
            result += std::size(fragments);
        }

        return result;
    }


    template<typename RouteID, Rank rank>
    std::set<RouteID> SerialRoutePartition<RouteID, rank>::route_ids() const
    {
        std::set<RouteID> result{};

        for (auto const& [route_id, fragments] : _route_fragments)
        {
            result.insert(route_id);
        }

        return result;
    }


    template<typename RouteID, Rank rank>
    auto SerialRoutePartition<RouteID, rank>::route_fragments(RouteID const route_id) const
        -> std::vector<RouteFragment>
    {
        auto const it = _route_fragments.find(route_id);

        lue_hpx_assert(it != _route_fragments.end());

        return it->second;
    }


    template<typename RouteID, Rank rank>
    auto SerialRoutePartition<RouteID, rank>::remote_route_fragment_locations() const
        -> std::vector<RouteFragmentLocation>
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


    template<typename RouteID, Rank rank>
    void SerialRoutePartition<RouteID, rank>::set_route_fragments(RouteFragments&& route_fragments)
    {
        _route_fragments = std::move(route_fragments);
    }


    template<typename RouteID, Rank rank>
    void SerialRoutePartition<RouteID, rank>::assert_invariants() const
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


// Register route partition action definitions for a route partition type
#define LUE_REGISTER_SERIAL_ROUTE_PARTITION_ACTIONS(RoutePartition)                                          \
                                                                                                             \
    HPX_REGISTER_ACTION(RoutePartition::OffsetAction, HPX_PP_CAT(RoutePartition, OffsetAction));             \
    HPX_REGISTER_ACTION(RoutePartition::ShapeAction, HPX_PP_CAT(RoutePartition, ShapeAction));               \
    HPX_REGISTER_ACTION(RoutePartition::NrRoutesAction, HPX_PP_CAT(RoutePartition, NrRoutesAction));         \
    HPX_REGISTER_ACTION(                                                                                     \
        RoutePartition::NrRouteFragmentsAction, HPX_PP_CAT(RoutePartition, NrRouteFragmentsAction));         \
    HPX_REGISTER_ACTION(RoutePartition::RouteIDsAction, HPX_PP_CAT(RoutePartition, RouteIDsAction));         \
    HPX_REGISTER_ACTION(                                                                                     \
        RoutePartition::RouteFragmentsAction, HPX_PP_CAT(RoutePartition, RouteFragmentsAction));             \
    HPX_REGISTER_ACTION(                                                                                     \
        RoutePartition::RemoteRouteFragmentLocationsAction,                                                  \
        HPX_PP_CAT(RoutePartition, RemoteRouteFragmentLocationsAction));                                     \
    HPX_REGISTER_ACTION(                                                                                     \
        RoutePartition::SetRouteFragmentsAction, HPX_PP_CAT(RoutePartition, SetRouteFragmentsAction));


#define LUE_REGISTER_SERIAL_ROUTE_PARTITION_COMPONENT(RoutePartition)                                        \
    using HPX_PP_CAT(RoutePartition, Component) = hpx::components::component<RoutePartition>;                \
    HPX_REGISTER_COMPONENT(HPX_PP_CAT(RoutePartition, Component));

// Register route partition definitions for an element type and rank
#define LUE_REGISTER_SERIAL_ROUTE_PARTITION(Element, rank)                                                   \
    using HPX_PP_CAT(RoutePartitionServer_, HPX_PP_CAT(Element, rank)) =                                     \
        lue::server::SerialRoutePartition<Element, rank>;                                                    \
                                                                                                             \
    LUE_REGISTER_SERIAL_ROUTE_PARTITION_COMPONENT(                                                           \
        HPX_PP_CAT(RoutePartitionServer_, HPX_PP_CAT(Element, rank)));                                       \
    LUE_REGISTER_SERIAL_ROUTE_PARTITION_ACTIONS(HPX_PP_CAT(RoutePartitionServer_, HPX_PP_CAT(Element, rank)));
