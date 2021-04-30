#include "lue/framework/core/component/server/array_partition_impl.hpp"


namespace lue::server {

    template class ArrayPartition<float, 0>;
    template class ArrayPartition<float, 1>;
    template class ArrayPartition<float, 2>;

}  // namespace lue::server
