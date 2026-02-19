#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array_decl.hpp"


namespace lue {

    template<typename Policies, typename Kernel>
        requires std::floating_point<policy::InputElementT<Policies, 0>> &&
                 std::floating_point<policy::OutputElementT<Policies, 0>> &&
                 std::same_as<policy::InputElementT<Policies, 0>, policy::OutputElementT<Policies, 0>> &&
                 std::integral<ElementT<Kernel>> && (rank<Kernel> == 2)
    auto focal_high_pass(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const& array,
        Kernel const& kernel) -> PartitionedArray<policy::OutputElementT<Policies, 0>, 2>;

}  // namespace lue
