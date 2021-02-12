#pragma once
#include "lue/framework/algorithm/all.hpp"


namespace lue::framework {

    template<
        typename Element,
        Rank rank>
    hpx::shared_future<Element> all(
        PartitionedArray<Element, rank> const& array)
    {
        using Policies = policy::all::DefaultValuePolicies<Element>;

        return all(Policies{}, array).share();
    }

}  // namespace lue::framework
