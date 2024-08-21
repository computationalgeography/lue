#include "lue/framework/algorithm/value_policies/logical_not.hpp"


namespace lue::framework {

    template<typename Element, Rank rank>
    auto logical_not(PartitionedArray<Element, rank> const& argument) -> PartitionedArray<std::uint8_t, rank>
    {
        return value_policies::logical_not<std::uint8_t>(argument);
    }

}  // namespace lue::framework
