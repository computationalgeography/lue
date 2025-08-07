#pragma once
#include "lue/framework/algorithm/resample.hpp"


namespace lue {
    namespace policy::downscale {

        template<typename Element>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::downscale


    namespace value_policies {

        template<typename Element, Rank rank>
        auto downscale(
            PartitionedArray<Element, rank> const& array, Count const count, DownscaleStrategy const strategy)
            -> PartitionedArray<Element, rank>
        {
            using Policies = policy::downscale::DefaultValuePolicies<Element>;

            return lue::downscale(Policies{}, array, count, strategy);
        }

    }  // namespace value_policies
}  // namespace lue
