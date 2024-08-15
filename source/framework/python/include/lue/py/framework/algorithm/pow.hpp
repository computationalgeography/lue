#pragma once
#include "lue/framework/algorithm/pow.hpp"


namespace lue::framework {

    template<typename Element, Rank rank, typename Argument1, typename Argument2>
    auto pow(Argument1 const& argument1, Argument2 const& argument2) -> PartitionedArray<Element, rank>
    {
        using Policies = policy::pow::DefaultValuePolicies<Element, Element>;

        return pow(Policies{}, argument1, argument2);
    }


    template<typename Element, typename Argument1, typename Argument2>
    auto pow(Argument1 const& argument1, Argument2 const& argument2) -> Scalar<Element>
    {
        return value_policies::pow(argument1, argument2);
    }

}  // namespace lue::framework
