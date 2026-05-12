#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array_decl.hpp"


namespace lue {

    template<typename Policies, typename Element, Rank rank>
        requires(
            std::floating_point<policy::InputElementT<Policies, 0>> &&
            std::floating_point<policy::InputElementT<Policies, 1>> &&
            std::same_as<policy::InputElementT<Policies, 0>, policy::InputElementT<Policies, 1>> &&
            std::floating_point<policy::OutputElementT<Policies, 0>>)
    auto atan2(
        Policies const& policies,
        PartitionedArray<Element, rank> const& array1,
        PartitionedArray<Element, rank> const& array2) -> PartitionedArray<Element, rank>;

}  // namespace lue
