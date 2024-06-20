#pragma once
#include "lue/framework/algorithm/value_policies/negate.hpp"


namespace lue::framework {

    template<typename Element, Rank rank>
    auto negate(PartitionedArray<Element, rank> const& array) -> PartitionedArray<Element, rank>
    {
        return value_policies::negate(array);
    }

}  // namespace lue::framework
