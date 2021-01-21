#include "lue/framework/core/component/array_partition.hpp"


// For an element type, register array partition actions for a number
// of ranks
#define LUE_REGISTER_ARRAY_PARTITIONS_ACTIONS(    \
        Element)                                  \
                                                  \
LUE_REGISTER_ARRAY_PARTITION_ACTIONS(Element, 0)  \
LUE_REGISTER_ARRAY_PARTITION_ACTIONS(Element, 1)  \
LUE_REGISTER_ARRAY_PARTITION_ACTIONS(Element, 2)

LUE_REGISTER_ARRAY_PARTITIONS_ACTIONS(bool)
LUE_REGISTER_ARRAY_PARTITIONS_ACTIONS(/* std:: */ uint8_t)
LUE_REGISTER_ARRAY_PARTITIONS_ACTIONS(/* std:: */ uint32_t)
LUE_REGISTER_ARRAY_PARTITIONS_ACTIONS(/* std:: */ uint64_t)
LUE_REGISTER_ARRAY_PARTITIONS_ACTIONS(/* std:: */ int32_t)
LUE_REGISTER_ARRAY_PARTITIONS_ACTIONS(/* std:: */ int64_t)
LUE_REGISTER_ARRAY_PARTITIONS_ACTIONS(float)
LUE_REGISTER_ARRAY_PARTITIONS_ACTIONS(double)

// #undef LUE_REGISTER_ARRAY_PARTITION_ACTIONS
#undef LUE_REGISTER_ARRAY_PARTITIONS_ACTIONS
