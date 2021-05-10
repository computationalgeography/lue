#include "lue/framework/partitioned_array/server/array_partition_impl.hpp"


namespace lue::server {

    template class ArrayPartition<bool, 0>;
    template class ArrayPartition<bool, 1>;
    template class ArrayPartition<bool, 2>;

}  // namespace lue::server
