#include "lue/framework/core/component/server/array_partition_impl.hpp"


namespace lue::server {

    template class ArrayPartition<std::uint8_t, 0>;
    template class ArrayPartition<std::uint8_t, 1>;
    template class ArrayPartition<std::uint8_t, 2>;

}  // namespace lue::server
