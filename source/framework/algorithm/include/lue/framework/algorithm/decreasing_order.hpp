#pragma once
#include "lue/framework/partitioned_array.hpp"
#include "lue/framework/serial_route.hpp"


namespace lue {
    namespace detail {

    }  // namespace detail


    template<typename Region, typename Element, Rank rank>
    SerialRoute<rank> decreasing_order(
        PartitionedArray<Region, rank> const& region,
        PartitionedArray<Element, rank> const& value,
        Count const max_length = std::numeric_limits<Count>::max())
    {
        // For each region, create a route from the cell with the highest value to the cell
        // with the lowest value. Stop when the route has the same length as the length passed in.

        /*
        For each partition, per zone sort the valid cells, keeping track of their location
        within the partition. This results in a collection of {zone: {value: cell_idxs}} records.
        For each partition this can execute in parallel. Return each partition's minimum values
        per zone.

        For each zone, determine the partition its minimum value is located in. Store this
        information in the resulting serial route instance and start storing route information
        in this start partition. Pass in the minima of all partitions for that zone.
        Continue storing route information as long as the local minimum is lower than the global
        one.
        Continue processing another partition once another lowest value is the next highest
        value. Continue until done, jumping from partition to partition. This is a serial
        operation per zone, but can be done in parallel for each zone.

        For each array partition, create a route partition containing cell indices, locality IDs,
        and starting points.

        Return the route.
        */
    }


    template<typename Element, Rank rank>
    SerialRoute<rank> decreasing_order(
        PartitionedArray<Element, rank> const& value,
        Count const max_length = std::numeric_limits<Count>::max())
    {
        // Create a single route from the cell with the highest value to the cell
        // with the lowest value. Stop when the route has the same length as the length passed in.
    }

}  // namespace lue
