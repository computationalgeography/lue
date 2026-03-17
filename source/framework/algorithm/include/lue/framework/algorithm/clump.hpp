#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array_decl.hpp"


namespace lue {

    enum class Connectivity { diagonal, nondiagonal };


    template<typename Policies, Rank rank>
    auto clump(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& zone,
        Connectivity connectivity) -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank>;

}  // namespace lue
