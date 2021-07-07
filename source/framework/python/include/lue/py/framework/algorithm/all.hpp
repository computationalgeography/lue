#pragma once
#include "lue/framework/algorithm/value_policies/all.hpp"


namespace lue::framework {

    template<
        typename Element,
        Rank rank>
    hpx::shared_future<Element> all(
        PartitionedArray<Element, rank> const& array)
    {
        return value_policies::all(array).share();
    }

}  // namespace lue::framework
