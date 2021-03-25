#include "lue/framework/algorithm/component/array_partition_io.hpp"
#include "lue/framework/algorithm/inter_partition_stream.hpp"
#include <hpx/serialization/std_tuple.hpp>


using lue_Index = lue::Index;
using lue_InterPartitionStreamMaterial = lue::InterPartitionStreamMaterial<lue::Index, 2>;


LUE_REGISTER_ARRAY_PARTITION_IO_ACTION_DECLARATIONS(lue_Index, 2, lue_InterPartitionStreamMaterial)


#define LUE_REGISTER(Value)                                   \
                                                              \
LUE_REGISTER_ARRAY_PARTITION_IO_ACTIONS(lue_Index, 2, Value)

LUE_REGISTER(/* std:: */ uint64_t)
LUE_REGISTER(float)
LUE_REGISTER(double)
LUE_REGISTER(lue_InterPartitionStreamMaterial)

#undef LUE_REGISTER
