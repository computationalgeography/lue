#pragma once
#include "lue/framework/algorithm/value_policies/multiply.hpp"


namespace lue::framework {

    template<typename Element, Rank rank, typename Argument1, typename Argument2>
    PartitionedArray<Element, rank> multiply(Argument1 const& argument1, Argument2 const& argument2)
    {
        return value_policies::multiply(argument1, argument2);
    }

}  // namespace lue::framework
