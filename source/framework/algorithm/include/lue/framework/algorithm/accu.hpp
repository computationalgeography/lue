#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/algorithm/scalar.hpp"
#include "lue/framework/partitioned_array.hpp"


namespace lue {

    // raster
    template<typename Policies>
    auto accu(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const& flow_direction,
        PartitionedArray<policy::InputElementT<Policies, 1>, 2> const& inflow)
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, 2>;


    // scalar
    template<typename Policies>
    auto accu(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const& flow_direction,
        Scalar<policy::InputElementT<Policies, 1>> const& inflow)
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, 2>;


    // value
    template<typename Policies>
    auto accu(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const& flow_direction,
        policy::InputElementT<Policies, 1> const& inflow)
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, 2>;

}  // namespace lue
