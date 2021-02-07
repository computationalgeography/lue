#pragma once
#include "lue/framework/algorithm/divide.hpp"


namespace lue::framework {

template<
    typename Element,
    Rank rank,
    typename Argument1,
    typename Argument2>
PartitionedArray<Element, rank> divide(
    Argument1 const& argument1,
    Argument2 const& argument2)
{
    using Policies = policy::divide::DefaultValuePolicies<Element, Element>;

    return divide(Policies{}, argument1, argument2);
}

}  // namespace lue::framework
