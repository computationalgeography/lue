#pragma once
#include "lue/framework/algorithm/definition/zonal_sum.hpp"
#include "lue/framework/algorithm/zonal_area.hpp"


namespace lue {

    template<typename Count, typename Policies, typename Zone, Rank rank>
    PartitionedArray<Count, rank> zonal_area(
        Policies const& policies, PartitionedArray<Zone, rank> const& zones)
    {
        using Functor = detail::ZonalSum<Count, Zone>;

        return zonal_operation(policies, Count{1}, zones, Functor{});
    }

}  // namespace lue


#define LUE_INSTANTIATE_ZONAL_AREA(Policies, Count, Zone)                                                    \
                                                                                                             \
    template LUE_ZONAL_OPERATION_EXPORT PartitionedArray<Count, 2>                                           \
    zonal_area<Count, ArgumentType<void(Policies)>, Zone, 2>(                                                \
        ArgumentType<void(Policies)> const&, PartitionedArray<Zone, 2> const&);
