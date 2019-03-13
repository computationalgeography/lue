#include "lue/framework/core/component/array_partition.hpp"


// Instantiate ArrayPartition component template class for various value
// types and ranks

#define LUE_REGISTER_ARRAY_PARTITIONS(type)  \
LUE_REGISTER_ARRAY_PARTITION(type, 0)        \
LUE_REGISTER_ARRAY_PARTITION(type, 1)        \
LUE_REGISTER_ARRAY_PARTITION(type, 2)

LUE_REGISTER_ARRAY_PARTITIONS(bool)

// LUE_REGISTER_ARRAY_PARTITIONS(/* std:: */ int8_t)
// LUE_REGISTER_ARRAY_PARTITIONS(/* std:: */ int16_t)
LUE_REGISTER_ARRAY_PARTITIONS(/* std:: */ int32_t)
LUE_REGISTER_ARRAY_PARTITIONS(/* std:: */ int64_t)

// LUE_REGISTER_ARRAY_PARTITIONS(/* std:: */ uint8_t)
// LUE_REGISTER_ARRAY_PARTITIONS(/* std:: */ uint16_t)
// LUE_REGISTER_ARRAY_PARTITIONS(/* std:: */ uint32_t)
// LUE_REGISTER_ARRAY_PARTITIONS(/* std:: */ uint64_t)

// LUE_REGISTER_ARRAY_PARTITIONS(float)
// LUE_REGISTER_ARRAY_PARTITIONS(double)

#undef LUE_REGISTER_ARRAY_PARTITIONS
