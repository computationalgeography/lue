#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array_decl.hpp"


namespace lue {

    template<typename Element>
    using Gradients = std::tuple<PartitionedArray<Element, 2>, PartitionedArray<Element, 2>>;


    template<typename Policies, typename Element>
    Gradients<Element> gradients(
        Policies const& policies, PartitionedArray<Element, 2> const& elevation, Element const cell_size);

}  // namespace lue
