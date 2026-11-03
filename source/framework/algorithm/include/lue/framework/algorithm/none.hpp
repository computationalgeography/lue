#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/algorithm/unary_aggregate_operation.hpp"


namespace lue {

    template<typename Policies, Rank rank>
    auto none(
        Policies const& policies, PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& array)
        -> Scalar<policy::OutputElementT<Policies, 0>>;

    template<typename Policies, Rank rank>
    auto none(
        hpx::id_type locality_id,
        Policies const& policies,
        ArrayPartition<policy::InputElementT<Policies, 0>, rank> const& partition)
        -> Scalar<policy::OutputElementT<Policies, 0>>;

}  // namespace lue
