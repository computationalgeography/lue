#pragma once
#include "lue/framework/algorithm/value_policies/add.hpp"


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
        return value_policies::add(argument1, argument2);
    }

}  // namespace lue::framework
