#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/algorithm/scalar.hpp"
#include "lue/framework/partitioned_array_decl.hpp"


namespace lue {

    /*!
        @brief      Accumulate @a inflow through @a flow_direction field and return result
    */
    template<typename Policies>
    auto accu(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const& flow_direction,
        PartitionedArray<policy::InputElementT<Policies, 1>, 2> const& inflow)
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, 2>;


    /*!
        @overload
    */
    template<typename Policies>
    auto accu(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const& flow_direction,
        Scalar<policy::InputElementT<Policies, 1>> const& inflow)
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, 2>;

}  // namespace lue
