#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/algorithm/scalar.hpp"
#include "lue/framework/partitioned_array.hpp"


namespace lue {

    template<typename Policies>
    auto accu_fraction(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const& flow_direction,
        PartitionedArray<policy::InputElementT<Policies, 1>, 2> const& inflow,
        PartitionedArray<policy::InputElementT<Policies, 2>, 2> const& fraction)
        -> std::tuple<
            PartitionedArray<policy::OutputElementT<Policies, 0>, 2>,
            PartitionedArray<policy::OutputElementT<Policies, 1>, 2>>;


    template<typename Policies>
    auto accu_fraction(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const& flow_direction,
        PartitionedArray<policy::InputElementT<Policies, 1>, 2> const& inflow,
        Scalar<policy::InputElementT<Policies, 2>> const& fraction)
        -> std::tuple<
            PartitionedArray<policy::OutputElementT<Policies, 0>, 2>,
            PartitionedArray<policy::OutputElementT<Policies, 1>, 2>>;


    template<typename Policies>
    auto accu_fraction(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const& flow_direction,
        Scalar<policy::InputElementT<Policies, 1>> const& inflow,
        Scalar<policy::InputElementT<Policies, 2>> const& fraction)
        -> std::tuple<
            PartitionedArray<policy::OutputElementT<Policies, 0>, 2>,
            PartitionedArray<policy::OutputElementT<Policies, 1>, 2>>;


    template<typename Policies>
    auto accu_fraction(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const& flow_direction,
        Scalar<policy::InputElementT<Policies, 1>> const& inflow,
        PartitionedArray<policy::InputElementT<Policies, 2>, 2> const& fraction)
        -> std::tuple<
            PartitionedArray<policy::OutputElementT<Policies, 0>, 2>,
            PartitionedArray<policy::OutputElementT<Policies, 1>, 2>>;

}  // namespace lue
