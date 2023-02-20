#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array.hpp"


namespace lue {

    template<typename BooleanElement, typename Policies, typename ExpressionElement, Rank rank>
    PartitionedArray<BooleanElement, rank> valid(
        Policies const& policies, PartitionedArray<ExpressionElement, rank> const& array);

}  // namespace lue
