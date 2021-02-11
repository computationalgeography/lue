#pragma once
#include "lue/framework/algorithm/array_partition_io.hpp"
#include <ostream>


namespace lue {

    template<
        typename Index,
        Rank rank,
        typename Value>
    std::ostream& operator<<(
        std::ostream& stream,
        ArrayPartitionIO<Index, rank, Value> const& partition_io)
    {
        stream
            << partition_io.input_cells_idxs() << "\n"
            << partition_io.values()
            ;

        return stream;
    }

}  // namespace lue
