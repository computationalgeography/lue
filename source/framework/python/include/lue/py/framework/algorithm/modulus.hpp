#pragma once
#include "lue/framework/algorithm/value_policies/modulus.hpp"


namespace lue::framework {

    template<typename Element, Rank rank, typename Argument1, typename Argument2>
    PartitionedArray<Element, rank> modulus(Argument1 const& argument1, Argument2 const& argument2)
    {
        return value_policies::modulus(argument1, argument2);
    }

}  // namespace lue::framework
