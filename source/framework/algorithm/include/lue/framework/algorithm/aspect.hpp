#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array_fwd.hpp"


namespace lue {

    template<typename Policies>
        requires std::floating_point<policy::InputElementT<Policies, 0>> &&
                 std::floating_point<policy::OutputElementT<Policies, 0>> &&
                 std::same_as<policy::InputElementT<Policies, 0>, policy::OutputElementT<Policies, 0>>
    auto aspect(
        Policies const& policies, PartitionedArray<policy::InputElementT<Policies, 0>, 2> const& elevation)
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, 2>;

}  // namespace lue
