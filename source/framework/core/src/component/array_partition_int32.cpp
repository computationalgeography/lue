#include "lue/framework/core/component/array_partition_impl.hpp"


namespace lue {

    template class ArrayPartition<std::int32_t, 0>;
    template class ArrayPartition<std::int32_t, 1>;
    template class ArrayPartition<std::int32_t, 2>;

}  // namespace lue
