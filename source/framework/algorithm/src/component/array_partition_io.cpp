#include "lue/framework/algorithm/component/array_partition_io.hpp"
#include "lue/framework/algorithm/definition/flow_accumulation.hpp"
#include "lue/framework/algorithm/definition/accu_info.hpp"
#include "lue/framework/algorithm/inter_partition_stream.hpp"
#include <hpx/serialization/std_tuple.hpp>


using lue_Index = lue::Index;
using lue_InterPartitionStreamMaterial = lue::InterPartitionStreamMaterial<lue::Index, 2>;
using lue_FlowAccumulationMaterial_float = lue::detail::flow_accumulation::Material<float>;
using lue_FlowAccumulationMaterial_double = lue::detail::flow_accumulation::Material<double>;
using lue_AccuInfoMaterial = lue::detail::accu_info::Accumulator<lue::Index, 2>::Material;


LUE_REGISTER_ARRAY_PARTITION_IO_ACTION_DECLARATIONS(lue_Index, 2, lue_InterPartitionStreamMaterial)
LUE_REGISTER_ARRAY_PARTITION_IO_ACTION_DECLARATIONS(lue_Index, 2, lue_FlowAccumulationMaterial_float)
LUE_REGISTER_ARRAY_PARTITION_IO_ACTION_DECLARATIONS(lue_Index, 2, lue_FlowAccumulationMaterial_double)
LUE_REGISTER_ARRAY_PARTITION_IO_ACTION_DECLARATIONS(lue_Index, 2, lue_AccuInfoMaterial)


#define LUE_REGISTER(Value)                                   \
                                                              \
LUE_REGISTER_ARRAY_PARTITION_IO_ACTIONS(lue_Index, 2, Value)

LUE_REGISTER(/* std:: */ uint64_t)
LUE_REGISTER(float)
LUE_REGISTER(double)
LUE_REGISTER(lue_InterPartitionStreamMaterial)
LUE_REGISTER(lue_FlowAccumulationMaterial_float)
LUE_REGISTER(lue_FlowAccumulationMaterial_double)
LUE_REGISTER(lue_AccuInfoMaterial)

#undef LUE_REGISTER
