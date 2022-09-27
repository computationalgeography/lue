#pragma once
#include "lue/framework/partitioned_array.hpp"


namespace lue {

    template<
        typename Element>
    PartitionedArray<Element, 2> read(
        std::string const& name,
        Shape<Count, 2> const& partition_shape);

    template<
        typename Element>
    hpx::future<void> write(
        PartitionedArray<Element, 2> const& array,
        std::string const& name,
        std::string const& clone_name);

}  // namespace lue
