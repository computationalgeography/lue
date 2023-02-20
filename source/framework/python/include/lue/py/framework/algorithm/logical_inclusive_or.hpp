#include "lue/framework/algorithm/value_policies/logical_inclusive_or.hpp"


namespace lue::framework {

    template<typename Element, Rank rank>
    PartitionedArray<std::uint8_t, rank> logical_inclusive_or(
        PartitionedArray<Element, rank> const& argument1, PartitionedArray<Element, rank> const& argument2)
    {
        return value_policies::logical_inclusive_or(argument1, argument2);
    }

}  // namespace lue::framework
