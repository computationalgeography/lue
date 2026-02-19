#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array_decl.hpp"


namespace lue {

    template<typename Policies, typename Element>
    PartitionedArray<Element, 2> slope(
        Policies const& policies, PartitionedArray<Element, 2> const& elevation, Element const cell_size);

}  // namespace lue
