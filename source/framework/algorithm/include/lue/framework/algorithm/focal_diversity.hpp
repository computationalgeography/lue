#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array_decl.hpp"


namespace lue {
    namespace policy::focal_diversity {

        template<std::integral Element>
        using DomainPolicy = AllValuesWithinDomain<Element>;

    }  // namespace policy::focal_diversity


    template<typename Policies, typename Kernel>
        requires std::integral<policy::InputElementT<Policies, 0>> &&
                 std::integral<policy::OutputElementT<Policies, 0>> && std::integral<ElementT<Kernel>> &&
                 (rank<Kernel> == 2)
    auto focal_diversity(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const& array,
        Kernel const& kernel) -> PartitionedArray<policy::OutputElementT<Policies, 0>, 2>;

}  // namespace lue
