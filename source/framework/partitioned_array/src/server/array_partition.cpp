#include "lue/framework/partitioned_array/server/array_partition_impl.hpp"


namespace lue::server {

}  // namespace lue::server


HPX_REGISTER_COMPONENT_MODULE()


#define LUE_REGISTER_ARRAY_PARTITION_ACTIONS(                         \
    Element,                                                          \
    rank)                                                             \
                                                                      \
HPX_REGISTER_COMPONENT(                                               \
    hpx::components::component<                                       \
        ArrayPartition_##Element##_##rank##_Component>,               \
    ArrayPartition_##Element##_##rank##_Component)                    \
                                                                      \
HPX_REGISTER_ACTION(                                                  \
    ArrayPartition_##Element##_##rank##_Component::DataAction,        \
    ArrayPartition_##Element##_##rank##_DataAction)                   \
                                                                      \
HPX_REGISTER_ACTION(                                                  \
    ArrayPartition_##Element##_##rank##_Component::SliceAction,       \
    ArrayPartition_##Element##_##rank##_SliceAction)                  \
                                                                      \
HPX_REGISTER_ACTION(                                                  \
    ArrayPartition_##Element##_##rank##_Component::FillAction,        \
    ArrayPartition_##Element##_##rank##_FillAction)                   \
                                                                      \
HPX_REGISTER_ACTION(                                                  \
    ArrayPartition_##Element##_##rank##_Component::SetDataAction,     \
    ArrayPartition_##Element##_##rank##_SetDataAction)                \
                                                                      \
HPX_REGISTER_ACTION(                                                  \
    ArrayPartition_##Element##_##rank##_Component::OffsetAction,      \
    ArrayPartition_##Element##_##rank##_OffsetAction)                 \
                                                                      \
HPX_REGISTER_ACTION(                                                  \
    ArrayPartition_##Element##_##rank##_Component::ShapeAction,       \
    ArrayPartition_##Element##_##rank##_ShapeAction)                  \
                                                                      \
HPX_REGISTER_ACTION(                                                  \
    ArrayPartition_##Element##_##rank##_Component::NrElementsAction,  \
    ArrayPartition_##Element##_##rank##_NrElementsAction)


#define LUE_REGISTER_ARRAY_PARTITIONS_ACTIONS(    \
    Element)                                      \
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


#undef LUE_REGISTER_ARRAY_PARTITIONS_ACTIONS
#undef LUE_REGISTER_ARRAY_PARTITION_ACTIONS
