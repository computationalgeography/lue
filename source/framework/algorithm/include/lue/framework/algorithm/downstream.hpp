#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array_decl.hpp"


namespace lue {

    template<typename Policies>
        requires std::integral<policy::InputElementT<Policies, 0>> &&
                 Arithmetic<policy::InputElementT<Policies, 1>> &&
                 Arithmetic<policy::OutputElementT<Policies, 0>> &&
                 std::same_as<policy::InputElementT<Policies, 1>, policy::OutputElementT<Policies, 0>>
    auto downstream(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const& flow_direction,
        PartitionedArray<policy::InputElementT<Policies, 1>, 2> const& material)
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, 2>;

}  // namespace lue
