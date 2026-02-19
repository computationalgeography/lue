#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/algorithm/scalar.hpp"
#include "lue/framework/partitioned_array_decl.hpp"


namespace lue {

    template<typename Policies>
    auto kinematic_wave(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const& flow_direction,
        PartitionedArray<policy::InputElementT<Policies, 1>, 2> const& current_outflow,
        PartitionedArray<policy::InputElementT<Policies, 2>, 2> const& inflow,
        PartitionedArray<policy::InputElementT<Policies, 3>, 2> const& alpha,
        PartitionedArray<policy::InputElementT<Policies, 4>, 2> const& beta,
        Scalar<policy::InputElementT<Policies, 5>> const& time_step_duration,
        PartitionedArray<policy::InputElementT<Policies, 6>, 2> const& channel_length)
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, 2>;


    template<typename Policies>
    auto kinematic_wave(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const& flow_direction,
        PartitionedArray<policy::InputElementT<Policies, 1>, 2> const& current_outflow,
        PartitionedArray<policy::InputElementT<Policies, 2>, 2> const& inflow,
        Scalar<policy::InputElementT<Policies, 3>> const& alpha,
        Scalar<policy::InputElementT<Policies, 4>> const& beta,
        Scalar<policy::InputElementT<Policies, 5>> const& time_step_duration,
        Scalar<policy::InputElementT<Policies, 6>> const& channel_length)
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, 2>;

}  // namespace lue
