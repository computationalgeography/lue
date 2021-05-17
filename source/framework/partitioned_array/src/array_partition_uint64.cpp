#include "lue/framework/partitioned_array/array_partition_impl.hpp"


namespace lue {

    template class ArrayPartition<std::uint64_t, 0>;
    template class ArrayPartition<std::uint64_t, 1>;
    template class ArrayPartition<std::uint64_t, 2>;

}  // namespace lue
