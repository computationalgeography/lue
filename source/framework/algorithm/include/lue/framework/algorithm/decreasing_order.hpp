#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array.hpp"
#include "lue/framework/serial_route.hpp"


namespace lue {

    template<typename Policies, typename Element, Rank rank>
    SerialRoute<rank> decreasing_order(
        Policies const& policies,
        PartitionedArray<Element, rank> const& value,
        Count const max_length = std::numeric_limits<Count>::max());


    template<typename Policies, typename Region, typename Element, Rank rank>
    SerialRoute<rank> decreasing_order(
        Policies const& policies,
        PartitionedArray<Region, rank> const& region,
        PartitionedArray<Element, rank> const& value,
        Count const max_length = std::numeric_limits<Count>::max());

}  // namespace lue
