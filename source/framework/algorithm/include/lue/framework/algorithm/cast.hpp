#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array.hpp"


namespace lue {

    template<
        typename OutputElement,
        typename Policies,
        typename InputElement,
        Rank rank>
    PartitionedArray<OutputElement, rank> cast(
        Policies const& policies,
        PartitionedArray<InputElement, rank> const& array);

}  // namespace lue
