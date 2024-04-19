#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/algorithm/scalar.hpp"
#include "lue/framework/partitioned_array.hpp"


namespace lue {

    template<typename BooleanElement, typename Policies, typename ExpressionElement, Rank rank>
    auto valid(Policies const& policies, PartitionedArray<ExpressionElement, rank> const& array)
        -> PartitionedArray<BooleanElement, rank>;

    template<typename BooleanElement, typename Policies, typename ExpressionElement>
    auto valid(Policies const& policies, Scalar<ExpressionElement> const& scalar) -> Scalar<BooleanElement>;

}  // namespace lue
