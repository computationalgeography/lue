#pragma once
#include "lue/framework/core/offset.hpp"
#include "lue/framework/core/shape.hpp"
#include "lue/framework/partitioned_route/serial_route_fragment.hpp"
#include "lue/framework/partitioned_route/serialize/serial_route_fragment.hpp"
#include "lue/framework/route/define.hpp"
#include <hpx/include/components.hpp>
#include <set>
#include <unordered_map>
#include <vector>


namespace lue::server {

    /*!
        @brief      Component server class for serial route partitions
    */
    template<typename RouteID, Rank rank>
    class HPX_COMPONENT_EXPORT SerialRoutePartition:
        public hpx::components::component_base<SerialRoutePartition<RouteID, rank>>
    {

        private:

            using Base = hpx::components::component_base<SerialRoutePartition<RouteID, rank>>;

        public:

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

            SerialRoutePartition& operator=(SerialRoutePartition const&) = default;

            SerialRoutePartition& operator=(SerialRoutePartition&&) = default;

            Offset offset() const;

            Shape shape() const;

            Count nr_routes() const;

            Count nr_route_fragments() const;

            std::set<RouteID> route_ids() const;

            std::vector<RouteFragment> route_fragments(RouteID const route_id) const;

            std::vector<RouteFragmentLocation> remote_route_fragment_locations() const;

            void set_route_fragments(RouteFragments&& route_fragments);

            HPX_DEFINE_COMPONENT_ACTION(SerialRoutePartition, offset, OffsetAction)

            HPX_DEFINE_COMPONENT_ACTION(SerialRoutePartition, shape, ShapeAction)

            HPX_DEFINE_COMPONENT_ACTION(SerialRoutePartition, nr_routes, NrRoutesAction)

            HPX_DEFINE_COMPONENT_ACTION(SerialRoutePartition, nr_route_fragments, NrRouteFragmentsAction)

            HPX_DEFINE_COMPONENT_ACTION(SerialRoutePartition, route_ids, RouteIDsAction)

            HPX_DEFINE_COMPONENT_ACTION(SerialRoutePartition, route_fragments, RouteFragmentsAction)

            HPX_DEFINE_COMPONENT_ACTION(
                SerialRoutePartition, remote_route_fragment_locations, RemoteRouteFragmentLocationsAction)


        private:

            void assert_invariants() const;

            //! Partition offset within the array
            Offset _offset;

            //! Partition shape
            Shape _shape;

            RouteFragments _route_fragments;
    };

}  // namespace lue::server


#define LUE_REGISTER_SERIAL_ROUTE_PARTITION_ACTION_DECLARATIONS(RouteID, rank, unique)                       \
                                                                                                             \
    namespace lue::detail {                                                                                  \
        using SerialRoutePartitionServer_##unique = lue::server::SerialRoutePartition<RouteID, rank>;        \
    }                                                                                                        \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        lue::detail::SerialRoutePartitionServer_##unique::OffsetAction,                                      \
        SerialRoutePartitionServerOffsetAction_##unique)                                                     \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        lue::detail::SerialRoutePartitionServer_##unique::ShapeAction,                                       \
        SerialRoutePartitionServerShapeAction_##unique)                                                      \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        lue::detail::SerialRoutePartitionServer_##unique::NrRoutesAction,                                    \
        SerialRoutePartitionServerNrRoutesAction_##unique)                                                   \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        lue::detail::SerialRoutePartitionServer_##unique::NrRouteFragmentsAction,                            \
        SerialRoutePartitionServerNrRouteFragmentsAction_##unique)                                           \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        lue::detail::SerialRoutePartitionServer_##unique::RouteIDsAction,                                    \
        SerialRoutePartitionServerRouteIDsAction_##unique)                                                   \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        lue::detail::SerialRoutePartitionServer_##unique::RouteFragmentsAction,                              \
        SerialRoutePartitionServerRouteFragmentsAction_##unique)                                             \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        lue::detail::SerialRoutePartitionServer_##unique::RemoteRouteFragmentLocationsAction,                \
        SerialRoutePartitionServerRemoteRouteFragmentLocationsAction_##unique)


#define LUE_REGISTER_SERIAL_ROUTE_PARTITION_ACTIONS(RouteID, rank, unique)                                   \
                                                                                                             \
    namespace lue::detail {                                                                                  \
        using SerialRoutePartitionServerComponent_##unique =                                                 \
            hpx::components::component<lue::detail::SerialRoutePartitionServer_##unique>;                    \
    }                                                                                                        \
                                                                                                             \
    HPX_REGISTER_COMPONENT(                                                                                  \
        lue::detail::SerialRoutePartitionServerComponent_##unique, lue_SerialRoutePartitionServer_##unique)  \
                                                                                                             \
    HPX_REGISTER_ACTION(                                                                                     \
        lue::detail::SerialRoutePartitionServer_##unique::OffsetAction,                                      \
        SerialRoutePartitionServerOffsetAction_##unique)                                                     \
                                                                                                             \
    HPX_REGISTER_ACTION(                                                                                     \
        lue::detail::SerialRoutePartitionServer_##unique::ShapeAction,                                       \
        SerialRoutePartitionServerShapeAction_##unique)                                                      \
                                                                                                             \
    HPX_REGISTER_ACTION(                                                                                     \
        lue::detail::SerialRoutePartitionServer_##unique::NrRoutesAction,                                    \
        SerialRoutePartitionServerNrRoutesAction_##unique)                                                   \
                                                                                                             \
    HPX_REGISTER_ACTION(                                                                                     \
        lue::detail::SerialRoutePartitionServer_##unique::NrRouteFragmentsAction,                            \
        SerialRoutePartitionServerNrRouteFragmentsAction_##unique)                                           \
                                                                                                             \
    HPX_REGISTER_ACTION(                                                                                     \
        lue::detail::SerialRoutePartitionServer_##unique::RouteIDsAction,                                    \
        SerialRoutePartitionServerRouteIDsAction_##unique)                                                   \
                                                                                                             \
    HPX_REGISTER_ACTION(                                                                                     \
        lue::detail::SerialRoutePartitionServer_##unique::RouteFragmentsAction,                              \
        SerialRoutePartitionServerRouteFragmentsAction_##unique)                                             \
                                                                                                             \
    HPX_REGISTER_ACTION(                                                                                     \
        lue::detail::SerialRoutePartitionServer_##unique::RemoteRouteFragmentLocationsAction,                \
        SerialRoutePartitionServerRemoteRouteFragmentLocationsAction_##unique)


#define LUE_REGISTER_SERIAL_ROUTE_PARTITION_ACTION_DECLARATIONS_RANK(Element)                                \
    LUE_REGISTER_SERIAL_ROUTE_PARTITION_ACTION_DECLARATIONS(Element, 1, Element##_1)                         \
    LUE_REGISTER_SERIAL_ROUTE_PARTITION_ACTION_DECLARATIONS(Element, 2, Element##_2)

#define LUE_REGISTER_SERIAL_ROUTE_PARTITION_ACTION_DECLARATIONS_ELEMENT()                                    \
    LUE_REGISTER_SERIAL_ROUTE_PARTITION_ACTION_DECLARATIONS_RANK(uint8_t)                                    \
    LUE_REGISTER_SERIAL_ROUTE_PARTITION_ACTION_DECLARATIONS_RANK(int32_t)                                    \
    LUE_REGISTER_SERIAL_ROUTE_PARTITION_ACTION_DECLARATIONS_RANK(uint32_t)                                   \
    LUE_REGISTER_SERIAL_ROUTE_PARTITION_ACTION_DECLARATIONS_RANK(int64_t)                                    \
    LUE_REGISTER_SERIAL_ROUTE_PARTITION_ACTION_DECLARATIONS_RANK(uint64_t)

LUE_REGISTER_SERIAL_ROUTE_PARTITION_ACTION_DECLARATIONS_ELEMENT()

#undef LUE_REGISTER_SERIAL_ROUTE_PARTITION_ACTION_DECLARATIONS
#undef LUE_REGISTER_SERIAL_ROUTE_PARTITION_ACTION_DECLARATIONS_ELEMENT
#undef LUE_REGISTER_SERIAL_ROUTE_PARTITION_ACTION_DECLARATIONS_RANK
