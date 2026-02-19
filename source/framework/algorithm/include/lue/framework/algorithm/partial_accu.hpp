#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/algorithm/scalar.hpp"
#include "lue/framework/partitioned_array_decl.hpp"


namespace lue {

    template<typename Policies>
    auto partial_accu(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const& flow_direction,
        Scalar<policy::InputElementT<Policies, 1>> const& inflow,
        Count nr_steps) -> PartitionedArray<policy::OutputElementT<Policies, 0>, 2>;

}  // namespace lue
