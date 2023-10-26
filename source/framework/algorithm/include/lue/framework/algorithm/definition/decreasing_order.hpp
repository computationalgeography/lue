#pragma once
#include "lue/framework/algorithm/component/decreasing_order_zone.hpp"
#include "lue/framework/algorithm/decreasing_order.hpp"
#include "lue/framework/algorithm/routing_operation_export.hpp"
#include "lue/macro.hpp"


namespace lue {
    namespace detail {

    }  // namespace detail


    template<typename Policies, typename Zone, typename Value, Rank rank>
    SerialRoute<rank> decreasing_order(
        Policies const& policies,
        PartitionedArray<Zone, rank> const& zone,
        PartitionedArray<Value, rank> const& value,
        Count const max_length)
    {
        // For each zone, create a route from the cell with the highest value to the cell
        // with the lowest value. Stop when the route has the same length as the length passed in.

        lue_hpx_assert(zone.shape() == value.shape());

        using Route = SerialRoute<rank>;
        using Partitions = PartitionsT2<Route>;
        using Shape = ShapeT<Route>;

        Shape const array_shape{value.shape()};
        Shape const shape_in_partitions{value.partitions().shape()};
        Count const nr_partitions{lue::nr_elements(value.partitions())};
        Partitions partitions{shape_in_partitions};

        using Component = DecreasingOrderZone<Zone, Value, rank>;

        // TODO hier verder
        // - Zet alle component spullen hier en in de .cpp voor deze operatie
        //     -
        //     https://hpx-docs.stellar-group.org/branches/master/html/manual/writing_distributed_hpx_applications.html#writing-components
        // - Ga verder met wat hieronder staat

        for (Index p = 0; p < nr_partitions; ++p)
        {
        }


        // For each partition, per zone sort the valid cells, keeping track of their location
        // within the partition. This results in a sorted collection of {value, cell_idxs}
        // records per zone.
        // For each partition this can execute in parallel.
        // Return each partition's maximum value per zone.

        // Logic for solving this:
        // [Worker locality]
        // - Iterate over all input partitions. Per partition:
        //     - Create a task on the remote locality which does the following:
        //         - Iterates over all zones and values. Per cell with a valid zone and value:
        //             - Create empty {value, cell_idxs} collection for the zone, if it does
        //               not already exist
        //             - Add the value to this collection
        //         - Create an empty {zone, value} map for storing the maximum value per zone
        //         - Iterate over all {value, cell_idxs} collections. Per collection:
        //             - Sort the {value, cell_idxs} tuples, by value, in decreasing order
        //             - Copy the highest value to the {zone, value} map
        //         - Return the {zone, value} map with maximum values
        //
        // [Root locality]
        // - Per partition, store the futures to the f{decreasing_order_client, f{zone, value}}
        //     information

        // Notes:
        // - The {zone: [{value, cell_idxs}]} collections must be kept alive after returning
        //     from the action. → We need a component + component actions for this
        //
        //     DecreasingOrderZone
        //     - Constructor. Creates empty collections. Members:
        //         - zone partition
        //         - value partition
        //         - collections...
        //     - sort_values(zone, value) → Fill collections, return maximum per zone
        //     - record_route(maxima, max_length)
        //
        //     DecreasingOrder
        //     - sort_values(value) → Fill collections, return local maximum
        //     - Constructor. Creates empty collections. Members:
        //         - value partition
        //         - collections...
        //     - sort_values(value) → Fill collections, return local maximum
        //     - record_route(maxima, max_length)


        // For each zone, determine the partition its maximum value is located in. Store this
        // information in the resulting serial route instance and start storing route information
        // in this start partition. Pass in the maxima of all partitions for that zone.

        // Continue storing route information as long as the current local maximum is higher
        // than the global one, located in another partition.

        // Once the local maximum is lower than the global one, continue with the procedure
        // for the partition that has the global maximum.

        // Continue until done, jumping from partition to partition. This is a serial operation
        // per zone, but can be done in parallel for each zone.

        // Keep max_length into account. Stop as soon as the current length of the route is
        // equal to the max_length passed in. Keep track of the length of the route while
        // recording it.


        // Logic for solving this:
        // [Root locality]
        // - Wait for all f{zone, value} information to be available for all partitions
        // - Create a new collection for storing {value, partition} information per zone: {zone,
        //   {value, partition}}. Sort this collection by value, in decreasing order.
        // - Iterate over all zones. Per zone:
        //     - Create a new route. The start of the route is determined by the cell with the
        //         highest value, returned by the previous step.
        //     - Spawn a task for recording the route, starting with the DecreasingOrder
        //         component accociated with the highest value just found. Pass in:
        //         - route_id
        //         - zone (one)
        //         - Sorted collection with {value, partition} records
        //         - max_length
        //     - Store the resulting f<void> in a collection
        //
        // [Remote locality]
        // - Create a SerialRouteFragment instance
        // - Append the cell_idxs for the current zone and first (highest) value to the fragment
        // - Pop this first value from the collection
        // - Continue while the new highest value is higher than or equal to the global one
        // - Once the new highest value is lower than the global one:
        //     - Append route fragment to the fragments collection for the current zone
        //         (vector<RouteFragment>)
        // - Note that collections may be accessed at the same time, for different zones. Make
        //   this possible.


        // When is the SerialRoutePartition created? When can it be marked as ready?
        // - record_route can return a route_ready future, for each route it is called for. Once
        //     all these futures are ready, all partitions can be marked as ready.
        //     This can be done as a one-shot side-effect of this operation, similar to how
        //     we do this in the flow accumulation operations. Meanwhile the resulting serial
        //     route can be returned already.

        // For each array partition, create a route partition containing cell indices, locality
        // IDs, and starting points.

        return SerialRoute<rank>{array_shape, std::move(partitions)};
    }


    template<typename Policies, typename Value, Rank rank>
    SerialRoute<rank> decreasing_order(
        Policies const& policies, PartitionedArray<Value, rank> const& value, Count const max_length)
    {
        // Create a single route from the cell with the highest value to the cell
        // with the lowest value. Stop when the route has the same length as the length passed in.

        lue_hpx_assert(value.shape() == value.shape());

        using Route = SerialRoute<rank>;
        using Partitions = PartitionsT2<Route>;
        using Shape = ShapeT<Route>;

        Shape const array_shape{value.shape()};
        Shape const shape_in_partitions{value.partitions().shape()};
        Count const nr_partitions{lue::nr_elements(value.partitions())};
        Partitions partitions{shape_in_partitions};

        for (Index p = 0; p < nr_partitions; ++p)
        {
        }

        return SerialRoute<rank>{array_shape, std::move(partitions)};
    }

}  // namespace lue


#define LUE_INSTANTIATE_DECREASING_ORDER(Policies, InputElement)                                             \
                                                                                                             \
    template LUE_ROUTING_OPERATION_EXPORT SerialRoute<2>                                                     \
    decreasing_order<ArgumentType<void(Policies)>, InputElement, 2>(                                         \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<InputElement, 2> const&,                                                            \
        Count const max_length);

#define LUE_INSTANTIATE_DECREASING_ORDER_ZONE(Policies, ZoneElement, InputElement)                           \
                                                                                                             \
    template LUE_ROUTING_OPERATION_EXPORT SerialRoute<2>                                                     \
    decreasing_order<ArgumentType<void(Policies)>, ZoneElement, InputElement, 2>(                            \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<ZoneElement, 2> const&,                                                             \
        PartitionedArray<InputElement, 2> const&,                                                            \
        Count const max_length);
