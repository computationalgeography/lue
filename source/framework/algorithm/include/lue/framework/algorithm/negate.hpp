#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array.hpp"


namespace lue {

    template<typename Policies, Rank rank>
    auto negate(
        Policies const& policies, PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& array)
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank>;

}  // namespace lue
