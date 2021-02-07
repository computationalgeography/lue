#pragma once
#include "lue/framework/algorithm/add.hpp"


namespace lue::framework {

template<
    typename Element,
    Rank rank,
    typename Argument1,
    typename Argument2>
PartitionedArray<Element, rank> add(
    Argument1 const& argument1,
    Argument2 const& argument2)
{
    using Policies = policy::add::DefaultValuePolicies<Element, Element>;

    return add(Policies{}, argument1, argument2);
}

}  // namespace lue::framework
