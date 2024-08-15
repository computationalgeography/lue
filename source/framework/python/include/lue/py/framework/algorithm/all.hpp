#pragma once
#include "lue/framework/algorithm/value_policies/all.hpp"


namespace lue::framework {

    template<typename Element, Rank rank>
    auto all(PartitionedArray<Element, rank> const& array) -> Scalar<Element>
    {
        return value_policies::all(array);
    }

}  // namespace lue::framework
