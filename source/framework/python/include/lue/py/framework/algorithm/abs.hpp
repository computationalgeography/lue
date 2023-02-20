#pragma once
#include "lue/framework/algorithm/value_policies/abs.hpp"


namespace lue::framework {

    template<typename Element, Rank rank>
    PartitionedArray<Element, rank> abs(PartitionedArray<Element, rank> const& array)
    {
        return value_policies::abs(array);
    }

}  // namespace lue::framework
