#include "lue/framework/partitioned_route/server/serial_route_partition_impl.hpp"


namespace lue::server {

}  // namespace lue::server


HPX_REGISTER_COMPONENT_MODULE()


#define LUE_REGISTER_SERIAL_ROUTE_PARTITION_ACTIONS(Element, rank)                                           \
                                                                                                             \
    HPX_REGISTER_COMPONENT(                                                                                  \
        hpx::components::component<SerialRoutePartition_##rank##_Component>,                                 \
        SerialRoutePartition_##rank##_Component)                                                             \
                                                                                                             \
    HPX_REGISTER_ACTION(                                                                                     \
        SerialRoutePartition_##rank##_Component::ShapeAction, SerialRoutePartition_##rank##_ShapeAction)     \
                                                                                                             \
    HPX_REGISTER_ACTION(                                                                                     \
        SerialRoutePartition_##rank##_Component::NrRoutesAction,                                             \
        SerialRoutePartition_##rank##_NrRoutesAction)                                                        \
                                                                                                             \
    HPX_REGISTER_ACTION(                                                                                     \
        SerialRoutePartition_##rank##_Component::NrRouteFragmentsAction,                                     \
        SerialRoutePartition_##rank##_NrRouteFragmentsAction)                                                \
                                                                                                             \
    HPX_REGISTER_ACTION(                                                                                     \
        SerialRoutePartition_##rank##_Component::RouteIDsAction,                                             \
        SerialRoutePartition_##rank##_RouteIDsAction)                                                        \
                                                                                                             \
    HPX_REGISTER_ACTION(                                                                                     \
        SerialRoutePartition_##rank##_Component::RouteFragmentsAction,                                       \
        SerialRoutePartition_##rank##_RouteFragmentsAction)


LUE_REGISTER_SERIAL_ROUTE_PARTITION_ACTIONS(Element, 1)
LUE_REGISTER_SERIAL_ROUTE_PARTITION_ACTIONS(Element, 2)

#undef LUE_REGISTER_SERIAL_ROUTE_PARTITION_ACTIONS
