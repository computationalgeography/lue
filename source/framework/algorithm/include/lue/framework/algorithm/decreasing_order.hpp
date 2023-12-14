#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array.hpp"
#include "lue/framework/serial_route.hpp"


namespace lue {

    template<typename Policies, Rank rank>
    auto decreasing_order(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 1>, rank> const& value,
        Count const max_length = std::numeric_limits<Count>::max())
        -> SerialRoute<policy::OutputElementT<Policies, 0>, rank>;


    template<typename Policies, Rank rank>
    auto decreasing_order(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& region,
        PartitionedArray<policy::InputElementT<Policies, 1>, rank> const& value,
        Count const max_length = std::numeric_limits<Count>::max())
        -> SerialRoute<policy::OutputElementT<Policies, 0>, rank>;

}  // namespace lue
