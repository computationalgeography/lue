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


    template<typename Index, Rank rank, typename Value>
    auto operator<<(std::ostream& stream, detail::ArrayPartitionIO<Index, rank, Value> const& partition_io)
        -> std::ostream&
    {
        stream << "partition_shape  : " << partition_io.partition_shape() << "\n"
               << "input_cells_idxs : " << partition_io.input_cells_idxs() << "\n"
               << "offsets          : " << partition_io.offsets() << "\n"
               << "values           :\n"
               << partition_io.values();

        return stream;
    }

}  // namespace lue
