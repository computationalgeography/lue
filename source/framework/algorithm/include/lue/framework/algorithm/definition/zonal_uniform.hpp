#pragma once
#include "lue/framework/algorithm/zonal_uniform.hpp"
#include "lue/framework/algorithm/zonal_operation_export.hpp"
#include "lue/framework/algorithm/definition/zonal_operation.hpp"
// #include <hpx/serialization/unordered_map.hpp>


namespace lue {

    template<
        typename Element,
        typename Policies,
        typename Zone,
        Rank rank>
    PartitionedArray<Element, rank> zonal_uniform(
        Policies const& policies,
        PartitionedArray<Zone, rank> const& zones)
    {
        // This function is different from the other zonal operations in that there is no
        // array argument. It is not needed. We only need a list of unique zones, associate a
        // uniform value with each zone, and reclass a partition given the zone - value map.
        // zonal_normal can do the same. Only difference is the way values are determined per zone.

        // Determine unique values in zones array passed in
        // TODO
        // unique_zones = unique(unique_policies, zones);

        // Associate a new value per zone
        // TODO
        // zone_value_map = ...

        // Reclass zones by values
        // TODO
        // return reclassify<Element>(policies, zone, zone_value_map);
    }

}  // namespace lue


#define LUE_INSTANTIATE_ZONAL_UNIFORM(                        \
    Policies, Element, Zone)                                  \
                                                              \
    template LUE_ZONAL_OPERATION_EXPORT                       \
    PartitionedArray<Element, 2> zonal_uniform<               \
            Element, ArgumentType<void(Policies)>, Zone, 2>(  \
        ArgumentType<void(Policies)> const&,                  \
        PartitionedArray<Zone, 2> const&);
