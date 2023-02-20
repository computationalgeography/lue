#pragma once
#include "lue/framework/algorithm/value_policies/less_than_equal_to.hpp"
#include "lue/py/framework/core/future_traits.hpp"


namespace lue::framework {

    template<typename InputElement, typename OutputElement, Rank rank, typename Argument1, typename Argument2>
    PartitionedArray<OutputElement, rank> less_than_equal_to(
        Argument1 const& argument1, Argument2 const& argument2)
    {
        if constexpr (!std::is_same_v<Argument1, InputElement>)
        {
            static_assert(std::is_same_v<ElementT<Argument1>, InputElement>);
        }

        if constexpr (!std::is_same_v<Argument2, InputElement>)
        {
            static_assert(std::is_same_v<ElementT<Argument2>, InputElement>);
        }

        return value_policies::less_than_equal_to(argument1, argument2);
    }

}  // namespace lue::framework
