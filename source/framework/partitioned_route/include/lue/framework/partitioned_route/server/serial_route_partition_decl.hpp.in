#pragma once
#include "lue/framework/core/offset.hpp"
#include "lue/framework/core/shape.hpp"
#include "lue/framework/partitioned_route/serial_route_fragment.hpp"
#include "lue/framework/partitioned_route/serialize/serial_route_fragment.hpp"
#include <hpx/include/components.hpp>
#include <hpx/preprocessor/cat.hpp>
#include <set>
#include <unordered_map>
#include <vector>


namespace lue::server {

    /*!
        @brief      Component server class for serial route partitions
    */
    template<typename RouteID, Rank rank>
    class SerialRoutePartition:
        public hpx::components::locking_hook<
            hpx::components::component_base<SerialRoutePartition<RouteID, rank>>>
    {

        public:

            using Base = hpx::components::locking_hook<
                hpx::components::component_base<SerialRoutePartition<RouteID, rank>>>;

            using RouteFragment = SerialRouteFragment<rank>;

            using RouteFragments = std::unordered_map<RouteID, std::vector<RouteFragment>>;

            using RouteFragmentLocation = typename RouteFragment::Location;

            using Offset = lue::Offset<Index, rank>;

            using Shape = lue::Shape<Index, rank>;

            SerialRoutePartition();

            SerialRoutePartition(Offset const& offset, Shape const& shape);

            SerialRoutePartition(Offset const& offset, Shape const& shape, RouteFragments&& route_fragments);

            SerialRoutePartition(SerialRoutePartition const&) = default;

            SerialRoutePartition(SerialRoutePartition&&) = default;

            ~SerialRoutePartition() = default;

            auto operator=(SerialRoutePartition const&) -> SerialRoutePartition& = default;

            auto operator=(SerialRoutePartition&&) -> SerialRoutePartition& = default;

            auto offset() const -> Offset;

            auto shape() const -> Shape;

            auto nr_routes() const -> Count;

            auto nr_route_fragments() const -> Count;

            auto route_ids() const -> std::set<RouteID>;

            auto route_fragments(RouteID route_id) const -> std::vector<RouteFragment>;

            auto remote_route_fragment_locations() const -> std::vector<RouteFragmentLocation>;

            void set_route_fragments(RouteFragments&& route_fragments);

            HPX_DEFINE_COMPONENT_ACTION(SerialRoutePartition, offset, OffsetAction);
            HPX_DEFINE_COMPONENT_ACTION(SerialRoutePartition, shape, ShapeAction);
            HPX_DEFINE_COMPONENT_ACTION(SerialRoutePartition, nr_routes, NrRoutesAction);
            HPX_DEFINE_COMPONENT_ACTION(SerialRoutePartition, nr_route_fragments, NrRouteFragmentsAction);
            HPX_DEFINE_COMPONENT_ACTION(SerialRoutePartition, route_ids, RouteIDsAction);
            HPX_DEFINE_COMPONENT_ACTION(SerialRoutePartition, route_fragments, RouteFragmentsAction);
            HPX_DEFINE_COMPONENT_ACTION(
                SerialRoutePartition, remote_route_fragment_locations, RemoteRouteFragmentLocationsAction);
            HPX_DEFINE_COMPONENT_ACTION(SerialRoutePartition, set_route_fragments, SetRouteFragmentsAction);

        private:

            void assert_invariants() const;

            //! Partition offset within the array
            Offset _offset;

            //! Partition shape
            Shape _shape;

            RouteFragments _route_fragments;
    };

}  // namespace lue::server


// Register route partition action declarations for a route partition type
#define LUE_REGISTER_SERIAL_ROUTE_PARTITION_ACTION_DECLARATIONS(RoutePartition)                              \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(RoutePartition::OffsetAction, HPX_PP_CAT(RoutePartition, OffsetAction)); \
    HPX_REGISTER_ACTION_DECLARATION(RoutePartition::ShapeAction, HPX_PP_CAT(RoutePartition, ShapeAction));   \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        RoutePartition::NrRoutesAction, HPX_PP_CAT(RoutePartition, NrRoutesAction));                         \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        RoutePartition::NrRouteFragmentsAction, HPX_PP_CAT(RoutePartition, NrRouteFragmentsAction));         \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        RoutePartition::RouteIDsAction, HPX_PP_CAT(RoutePartition, RouteIDsAction));                         \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        RoutePartition::RouteFragmentsAction, HPX_PP_CAT(RoutePartition, RouteFragmentsAction));             \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        RoutePartition::RemoteRouteFragmentLocationsAction,                                                  \
        HPX_PP_CAT(RoutePartition, RemoteRouteFragmentLocationsAction));                                     \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        RoutePartition::SetRouteFragmentsAction, HPX_PP_CAT(RoutePartition, SetRouteFragmentsAction));
