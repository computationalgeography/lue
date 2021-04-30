#include "lue/framework/core/component/server/array_partition_impl.hpp"


namespace lue::server {

    template class ArrayPartition<std::int32_t, 0>;
    template class ArrayPartition<std::int32_t, 1>;
    template class ArrayPartition<std::int32_t, 2>;

}  // namespace lue::server
