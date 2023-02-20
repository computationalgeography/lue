#include "lue/framework/algorithm/value_policies/logical_not.hpp"


namespace lue::framework {

    template<typename Element, Rank rank>
    PartitionedArray<std::uint8_t, rank> logical_not(PartitionedArray<Element, rank> const& argument)
    {
        return value_policies::logical_not(argument);
    }

}  // namespace lue::framework
