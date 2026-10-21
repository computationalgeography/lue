#pragma once
#include "lue/framework/algorithm/inter_partition_stream.hpp"
#include "lue/framework/test/stream.hpp"
#include <ostream>


namespace lue {

    template<typename Index, Rank rank>
    auto operator<<(std::ostream& stream, InterPartitionStreamMaterial<Index, rank> const& material)
        -> std::ostream&
    {
        stream << "  input_cell_idxs: " << material.input_cells_idxs() << "\n"
               << "  stream_class   : " << material.stream_class() << "\n"
               << "  count          : " << material.count() << "\n";

        return stream;
    }

}  // namespace lue
