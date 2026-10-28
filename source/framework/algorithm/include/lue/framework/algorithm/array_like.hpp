#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/algorithm/scalar.hpp"
#include "lue/framework/partitioned_array_decl.hpp"
#include "lue/concept.hpp"


namespace lue {

    template<Arithmetic OutputElement, typename Policies, Arithmetic InputElement, Rank rank>
    auto array_like(
        Policies const& policies,
        PartitionedArray<InputElement, rank> const& input_array,
        Scalar<OutputElement> const& fill_value) -> PartitionedArray<OutputElement, rank>;


    template<Arithmetic OutputElement, typename Policies, Arithmetic InputElement, Rank rank>
    auto array_like(
        Policies const& policies,
        PartitionedArray<InputElement, rank> const& input_array,
        OutputElement fill_value) -> PartitionedArray<OutputElement, rank>
    {
        return array_like(policies, input_array, Scalar{fill_value});
    }

}  // namespace lue
