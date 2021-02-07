#pragma once
#include "lue/framework/algorithm/multiply.hpp"


namespace lue::framework {

template<
    typename Element,
    Rank rank,
    typename Argument1,
    typename Argument2>
PartitionedArray<Element, rank> multiply(
    Argument1 const& argument1,
    Argument2 const& argument2)
{
    using Policies = policy::multiply::DefaultValuePolicies<Element, Element>;

    return multiply(Policies{}, argument1, argument2);
}

}  // namespace lue::framework
