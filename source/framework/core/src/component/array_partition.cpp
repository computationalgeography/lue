#include "lue/framework/core/component/array_partition.hpp"


#define LUE_REGISTER_ARRAY_PARTITION_ACTIONS(                       \
        Element,                                                    \
        rank)                                                       \
                                                                    \
namespace lue {                                                     \
namespace detail {                                                  \
                                                                    \
using ArrayPartitionServerType_##Element##_##rank =                 \
    hpx::components::component<ArrayPartition_##Element##_##rank>;  \
                                                                    \
}                                                                   \
}                                                                   \
                                                                    \
HPX_REGISTER_COMPONENT(                                             \
    lue::detail::ArrayPartitionServerType_##Element##_##rank,       \
    ArrayPartitionServer_##Element##_##rank)                        \
                                                                    \
HPX_REGISTER_ACTION(                                                \
    lue::detail::ArrayPartition_##Element##_##rank::DataAction,     \
    ArrayPartition_##Element##_##rank##_DataAction)                 \
                                                                    \
HPX_REGISTER_ACTION(                                                \
    lue::detail::ArrayPartition_##Element##_##rank::FillAction,     \
    ArrayPartition_##Element##_##rank##_FillAction)                 \
                                                                    \
HPX_REGISTER_ACTION(                                                \
    lue::detail::ArrayPartition_##Element##_##rank::ShapeAction,    \
    ArrayPartition_##Element##_##rank##_ShapeAction)                \
                                                                    \
HPX_REGISTER_ACTION(                                                \
    lue::detail::ArrayPartition_##Element##_##rank::SizeAction,     \
    ArrayPartition_##Element##_##rank##_SizeAction)


// For an element type, register array partition actions for a number
// of ranks
#define LUE_REGISTER_ARRAY_PARTITIONS_ACTIONS(    \
        Element)                                  \
                                                  \
LUE_REGISTER_ARRAY_PARTITION_ACTIONS(Element, 0)  \
LUE_REGISTER_ARRAY_PARTITION_ACTIONS(Element, 1)  \
LUE_REGISTER_ARRAY_PARTITION_ACTIONS(Element, 2)

LUE_REGISTER_ARRAY_PARTITIONS_ACTIONS(/* std:: */ int32_t)
LUE_REGISTER_ARRAY_PARTITIONS_ACTIONS(/* std:: */ int64_t)

#undef LUE_REGISTER_ARRAY_PARTITION_ACTIONS
#undef LUE_REGISTER_ARRAY_PARTITIONS_ACTIONS
