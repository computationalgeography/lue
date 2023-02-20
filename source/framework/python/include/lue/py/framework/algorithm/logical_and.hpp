#include "lue/framework/algorithm/value_policies/logical_and.hpp"


namespace lue::framework {

    template<typename Element, Rank rank>
    PartitionedArray<std::uint8_t, rank> logical_and(
        PartitionedArray<Element, rank> const& argument1, PartitionedArray<Element, rank> const& argument2)
    {
        return value_policies::logical_and(argument1, argument2);
    }

}  // namespace lue::framework
