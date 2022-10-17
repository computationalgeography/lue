#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array.hpp"


namespace lue {

    template<
        typename Policies,
        typename Element,
        Rank rank>
    PartitionedArray<std::uint8_t, rank> logical_not(
        Policies const& policies,
        PartitionedArray<Element, rank> const& array);

}  // namespace lue
