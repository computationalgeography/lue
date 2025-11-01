#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array.hpp"


namespace lue {
    namespace policy::focal_diversity {

        template<std::integral Element>
        using DomainPolicy = AllValuesWithinDomain<Element>;

    }  // namespace policy::focal_diversity


    template<typename Policies, typename Kernel>
    auto focal_diversity(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const& array,
        Kernel const& kernel) -> PartitionedArray<policy::OutputElementT<Policies, 0>, 2>;

}  // namespace lue
