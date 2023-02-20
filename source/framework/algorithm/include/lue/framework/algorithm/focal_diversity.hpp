#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array.hpp"


namespace lue {
    namespace policy::focal_diversity {

        template<typename Element>
        using DomainPolicy = AllValuesWithinDomain<Element>;

    }  // namespace policy::focal_diversity


    template<typename Count, typename Policies, typename Element, Rank rank, typename Kernel>
    PartitionedArray<Count, rank> focal_diversity(
        Policies const& policies, PartitionedArray<Element, rank> const& array, Kernel const& kernel);

}  // namespace lue
