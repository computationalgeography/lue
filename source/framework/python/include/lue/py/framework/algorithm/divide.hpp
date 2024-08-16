#pragma once
#include "lue/framework/algorithm/value_policies/divide.hpp"


namespace lue::framework {

    template<typename Element, Rank rank, typename Argument1, typename Argument2>
    auto divide(Argument1 const& argument1, Argument2 const& argument2) -> PartitionedArray<Element, rank>
    {
        return value_policies::divide(argument1, argument2);
    }


    template<typename Element, typename Argument1, typename Argument2>
    auto divide(Argument1 const& argument1, Argument2 const& argument2) -> Scalar<Element>
    {
        return value_policies::divide(argument1, argument2);
    }

}  // namespace lue::framework
