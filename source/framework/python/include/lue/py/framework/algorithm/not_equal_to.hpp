#pragma once
#include "lue/framework/algorithm/value_policies/not_equal_to.hpp"
#include "lue/py/framework/core/future_traits.hpp"


namespace lue::framework {

    template<typename OutputElement, Rank rank, typename Argument1, typename Argument2>
    auto not_equal_to(Argument1 const& argument1, Argument2 const& argument2)
        -> PartitionedArray<OutputElement, rank>
    {
        return value_policies::not_equal_to(argument1, argument2);
    }


    template<typename OutputElement, typename Argument1, typename Argument2>
    auto not_equal_to(Argument1 const& argument1, Argument2 const& argument2) -> Scalar<OutputElement>
    {
        return value_policies::not_equal_to(argument1, argument2);
    }

}  // namespace lue::framework
