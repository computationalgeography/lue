#pragma once
#include "lue/framework/algorithm/array_partition_io.hpp"
#include "lue/framework/core/component/server/array_partition.hpp"


#define MAH(rank, Value) \
    using ArrayPartitionIO_##rank##_##Value = lue::ArrayPartitionIO<lue::Index, rank, Value>; \
    LUE_REGISTER_ARRAY_PARTITION_ACTION_DECLARATIONS(ArrayPartitionIO_##rank##_##Value, rank)

MAH(2, uint64_t)

#undef MAH
