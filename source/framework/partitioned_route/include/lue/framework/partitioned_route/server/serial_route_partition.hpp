#pragma once
#include "lue/framework/core/offset.hpp"
#include "lue/framework/core/shape.hpp"
#include "lue/framework/partitioned_route/serial_route_fragment.hpp"
#include "lue/framework/partitioned_route/serialize/serial_route_fragment.hpp"
#include <hpx/include/components.hpp>
#include <unordered_map>


namespace lue::server {

    /*!
        @brief      Component server class for serial route partitions
    */
    template<Rank rank>
    class HPX_COMPONENT_EXPORT SerialRoutePartition:
        public hpx::components::component_base<SerialRoutePartition<rank>>
    {

        private:

            using Base = hpx::components::component_base<SerialRoutePartition<rank>>;

        public:

            //! ID of a route
            using RouteID = Index;

            using RouteFragment = SerialRouteFragment<rank>;

            using RouteFragments = std::unordered_map<RouteID, std::vector<RouteFragment>>;

            using Offset = lue::Offset<Index, rank>;

            using Shape = lue::Shape<Index, rank>;

            SerialRoutePartition();

            SerialRoutePartition(Offset const& offset, Shape const& shape, RouteFragments&& route_fragments);

            SerialRoutePartition(SerialRoutePartition const&) = default;

            SerialRoutePartition(SerialRoutePartition&&) = default;

            ~SerialRoutePartition() = default;

            SerialRoutePartition& operator=(SerialRoutePartition const&) = default;

            SerialRoutePartition& operator=(SerialRoutePartition&&) = default;

            Offset offset() const;

            Shape shape() const;

            Count nr_routes() const;

            Count nr_route_fragments() const;

            std::vector<RouteID> route_ids() const;

            std::vector<RouteFragment> route_fragments(RouteID const route_id) const;

            HPX_DEFINE_COMPONENT_ACTION(SerialRoutePartition, shape, OffsetAction)

            HPX_DEFINE_COMPONENT_ACTION(SerialRoutePartition, shape, ShapeAction)

            HPX_DEFINE_COMPONENT_ACTION(SerialRoutePartition, nr_routes, NrRoutesAction)

            HPX_DEFINE_COMPONENT_ACTION(SerialRoutePartition, nr_route_fragments, NrRouteFragmentsAction)

            HPX_DEFINE_COMPONENT_ACTION(SerialRoutePartition, route_ids, RouteIDsAction)

            HPX_DEFINE_COMPONENT_ACTION(SerialRoutePartition, route_fragments, RouteFragmentsAction)


        private:

            void assert_invariants() const;

            //! Partition offset within the array
            Offset _offset;

            //! Partition shape
            Shape _shape;

            RouteFragments _route_fragments;
    };

}  // namespace lue::server


#define LUE_REGISTER_SERIAL_ROUTE_PARTITION_ACTION_DECLARATIONS(rank)                                        \
                                                                                                             \
    using SerialRoutePartition_##rank##_Component = lue::server::SerialRoutePartition<rank>;                 \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        SerialRoutePartition_##rank##_Component::OffsetAction, SerialRoutePartition_##rank##_OffsetAction)   \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        SerialRoutePartition_##rank##_Component::ShapeAction, SerialRoutePartition_##rank##_ShapeAction)     \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        SerialRoutePartition_##rank##_Component::NrRoutesAction,                                             \
        SerialRoutePartition_##rank##_NrRoutesAction)                                                        \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        SerialRoutePartition_##rank##_Component::NrRouteFragmentsAction,                                     \
        SerialRoutePartition_##rank##_NrRouteFragmentsAction)                                                \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        SerialRoutePartition_##rank##_Component::RouteIDsAction,                                             \
        SerialRoutePartition_##rank##_RouteIDsAction)                                                        \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        SerialRoutePartition_##rank##_Component::RouteFragmentsAction,                                       \
        SerialRoutePartition_##rank##_RouteFragmentsAction)


LUE_REGISTER_SERIAL_ROUTE_PARTITION_ACTION_DECLARATIONS(1)
LUE_REGISTER_SERIAL_ROUTE_PARTITION_ACTION_DECLARATIONS(2)

#undef LUE_REGISTER_SERIAL_ROUTE_PARTITION_ACTION_DECLARATIONS
