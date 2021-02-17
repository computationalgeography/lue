#pragma once
#include "lue/framework/algorithm/pow.hpp"


namespace lue::framework {

    template<
        typename Element,
        Rank rank,
        typename Argument1,
        typename Argument2>
    PartitionedArray<Element, rank> pow(
        Argument1 const& argument1,
        Argument2 const& argument2)
    {
        using Policies = policy::pow::DefaultValuePolicies<Element, Element>;

        return pow(Policies{}, argument1, argument2);
    }

}  // namespace lue::framework
