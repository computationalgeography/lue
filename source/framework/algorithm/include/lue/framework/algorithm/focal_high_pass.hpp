#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array.hpp"


namespace lue {

    template<typename Policies, typename Kernel>
        requires std::is_floating_point_v<policy::InputElementT<Policies, 0>> &&
                 std::is_floating_point_v<policy::OutputElementT<Policies, 0>> &&
                 std::is_same_v<policy::InputElementT<Policies, 0>, policy::OutputElementT<Policies, 0>> &&
                 std::is_integral_v<ElementT<Kernel>> && (rank<Kernel> == 2)
    auto focal_high_pass(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const& array,
        Kernel const& kernel) -> PartitionedArray<policy::OutputElementT<Policies, 0>, 2>;

}  // namespace lue
