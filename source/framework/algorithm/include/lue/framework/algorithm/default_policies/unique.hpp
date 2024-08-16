#pragma once
#include "lue/framework/algorithm/unique.hpp"


namespace lue {
    namespace policy::unique {

        template<typename Element>
        using DefaultPolicies = policy::
            DefaultPolicies<AllValuesWithinDomain<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::unique


    namespace default_policies {

        template<typename Element, Rank rank>
        auto unique(PartitionedArray<Element, rank> const& array) -> hpx::future<std::set<Element>>
        {
            using Policies = policy::unique::DefaultPolicies<Element>;

            return lue::unique(Policies{}, array);
        }

    }  // namespace default_policies
}  // namespace lue
