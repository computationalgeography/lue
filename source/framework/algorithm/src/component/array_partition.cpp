#include "lue/framework/algorithm/component/server/array_partition.hpp"
#include "lue/framework/core/component/array_partition.hpp"


#define MEH(rank, Value) \
    using ArrayPartitionIO_##rank##_##Value = lue::ArrayPartitionIO<lue::Index, rank, Value>; \
    LUE_REGISTER_ARRAY_PARTITION_ACTIONS(ArrayPartitionIO_##rank##_##Value, rank)

MEH(2, uint64_t)

#undef MEH
