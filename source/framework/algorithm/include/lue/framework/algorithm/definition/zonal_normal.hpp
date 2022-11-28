#pragma once
#include "lue/framework/algorithm/zonal_normal.hpp"
#include "lue/framework/algorithm/zonal_operation_export.hpp"
#include "lue/framework/algorithm/definition/zonal_operation.hpp"
// #include <hpx/serialization/unordered_map.hpp>


namespace lue {

    template<
        typename Element,
        typename Policies,
        typename Zone,
        Rank rank>
    PartitionedArray<Element, rank> zonal_normal(
        Policies const& policies,
        PartitionedArray<Zone, rank> const& zones)
    {
        // using Functor = detail::ZonalSum<Element, Zone>;

        // return zonal_operation(policies, Element{1}, zones, Functor{});
    }

}  // namespace lue


#define LUE_INSTANTIATE_ZONAL_NORMAL(                         \
    Policies, Element, Zone)                                  \
                                                              \
    template LUE_ZONAL_OPERATION_EXPORT                       \
    PartitionedArray<Element, 2> zonal_normal<                \
            Element, ArgumentType<void(Policies)>, Zone, 2>(  \
        ArgumentType<void(Policies)> const&,                  \
        PartitionedArray<Zone, 2> const&);
