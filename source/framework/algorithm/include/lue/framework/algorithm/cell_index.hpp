#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/configure.hpp"
#include "lue/framework/partitioned_array_fwd.hpp"


namespace lue {

    template<typename Policies, Rank rank>
        requires std::same_as<policy::InputElementT<Policies, 0>, BooleanElement>
    auto cell_index(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& condition,
        Index dimension_idx) -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank>;

}  // namespace lue
