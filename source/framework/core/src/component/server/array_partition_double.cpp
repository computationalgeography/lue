#include "lue/framework/core/component/server/array_partition_impl.hpp"


namespace lue::server {

    template class ArrayPartition<double, 0>;
    template class ArrayPartition<double, 1>;
    template class ArrayPartition<double, 2>;

}  // namespace lue::server
