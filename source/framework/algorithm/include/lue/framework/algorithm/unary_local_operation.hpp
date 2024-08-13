#pragma once
#include "lue/framework/algorithm/functor_traits.hpp"
#include "lue/framework/algorithm/scalar.hpp"
#include "lue/framework/partitioned_array.hpp"


namespace lue {

    // local_operation(array, array)
    template<typename Policies, typename InputElement, Rank rank, typename Functor>
    auto unary_local_operation(
        Policies const& policies,
        PartitionedArray<InputElement, rank> const& input_array,
        Functor const& functor) -> PartitionedArray<OutputElementT<Functor>, rank>;


    // local_operation(scalar, scalar)
    template<typename Policies, typename InputElement, typename Functor>
    auto unary_local_operation(
        Policies const& policies, Scalar<InputElement> const& input_scalar, Functor const& functor)
        -> Scalar<OutputElementT<Functor>>;

}  // namespace lue
