#pragma once
#include "lue/framework/algorithm/equal_to.hpp"
#include "lue/py/framework/core/future_traits.hpp"


namespace lue::framework {

template<
    typename InputElement,
    typename OutputElement,
    Rank rank,
    typename Argument1,
    typename Argument2>
PartitionedArray<OutputElement, rank> equal_to(
    Argument1 const& argument1,
    Argument2 const& argument2)
{
    if constexpr (!std::is_same_v<Argument1, InputElement>)
    {
        static_assert(std::is_same_v<ElementT<Argument1>, InputElement>);
    }

    if constexpr (!std::is_same_v<Argument2, InputElement>)
    {
        static_assert(std::is_same_v<ElementT<Argument2>, InputElement>);
    }

    using Policies = policy::equal_to::DefaultValuePolicies<OutputElement, InputElement>;

    return equal_to(Policies{}, argument1, argument2);
}

}  // namespace lue::framework
