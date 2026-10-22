#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array_decl.hpp"


namespace lue {

    template<typename Policies>
        requires std::floating_point<policy::InputElementT<Policies, 0>> &&
                 std::integral<policy::OutputElementT<Policies, 0>>
    auto d8_flow_direction(
        Policies const& policies, PartitionedArray<policy::InputElementT<Policies, 0>, 2> const& elevation)
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, 2>;

}  // namespace lue
