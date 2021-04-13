#include "lue/framework/algorithm/component/array_partition_io.hpp"
#include "lue/framework/algorithm/accu_fraction.hpp"
#include "lue/framework/algorithm/accu_threshold.hpp"
#include "lue/framework/algorithm/inter_partition_stream.hpp"
#include <hpx/serialization/std_tuple.hpp>


using lue_Index = lue::Index;
using lue_InterPartitionStreamMaterial = lue::InterPartitionStreamMaterial<lue::Index, 2>;
using lue_AccuFractionMaterial_float = lue::detail::AccuFractionMaterial<float, float>;
using lue_AccuFractionMaterial_double = lue::detail::AccuFractionMaterial<double, double>;
using lue_AccuThresholdMaterial_float = lue::detail::AccuThresholdMaterial<float>;
using lue_AccuThresholdMaterial_double = lue::detail::AccuThresholdMaterial<double>;


LUE_REGISTER_ARRAY_PARTITION_IO_ACTION_DECLARATIONS(lue_Index, 2, lue_InterPartitionStreamMaterial)
LUE_REGISTER_ARRAY_PARTITION_IO_ACTION_DECLARATIONS(lue_Index, 2, lue_AccuFractionMaterial_float)
LUE_REGISTER_ARRAY_PARTITION_IO_ACTION_DECLARATIONS(lue_Index, 2, lue_AccuFractionMaterial_double)
LUE_REGISTER_ARRAY_PARTITION_IO_ACTION_DECLARATIONS(lue_Index, 2, lue_AccuThresholdMaterial_float)
LUE_REGISTER_ARRAY_PARTITION_IO_ACTION_DECLARATIONS(lue_Index, 2, lue_AccuThresholdMaterial_double)


#define LUE_REGISTER(Value)                                   \
                                                              \
LUE_REGISTER_ARRAY_PARTITION_IO_ACTIONS(lue_Index, 2, Value)

LUE_REGISTER(/* std:: */ uint64_t)
LUE_REGISTER(float)
LUE_REGISTER(double)
LUE_REGISTER(lue_InterPartitionStreamMaterial)
LUE_REGISTER(lue_AccuFractionMaterial_float)
LUE_REGISTER(lue_AccuFractionMaterial_double)
LUE_REGISTER(lue_AccuThresholdMaterial_float)
LUE_REGISTER(lue_AccuThresholdMaterial_double)

#undef LUE_REGISTER
