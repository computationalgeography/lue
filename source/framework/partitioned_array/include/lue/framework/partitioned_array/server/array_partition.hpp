#pragma once
#include "lue/framework/core/offset.hpp"
#include "lue/framework/partitioned_array/array_partition_data.hpp"
#include <hpx/config.hpp>
#include <hpx/include/components.hpp>


namespace lue::server {

    /*!
        @brief      Component server class for partioned array partitions
    */
    template<typename Element, Rank rank>
    class HPX_COMPONENT_EXPORT ArrayPartition:
        public hpx::components::component_base<ArrayPartition<Element, rank>>
    {

        private:

            using Base = hpx::components::component_base<ArrayPartition<Element, rank>>;

        public:

            using Data = ArrayPartitionData<Element, rank>;

            using Shape = typename Data::Shape;

            using Offset = lue::Offset<Index, rank>;

            using Slice = typename Data::Slice;

            using Slices = typename Data::Slices;


            ArrayPartition(Offset const& offset, Shape const& shape);

            ArrayPartition(Offset const& offset, Shape const& shape, Element value);

            ArrayPartition(Offset const& offset, Data const& data);

            ArrayPartition(Offset const& offset, Data&& data);

            ArrayPartition(ArrayPartition const& other);

            ArrayPartition(ArrayPartition&& other);

            ~ArrayPartition() = default;

            ArrayPartition& operator=(ArrayPartition const&) = delete;

            ArrayPartition& operator=(ArrayPartition&&) = delete;

            Offset offset() const;

            Data data() const;

            Data slice([[maybe_unused]] Slices const& slices) const;

            void fill(Element value);

            void set_data(Data const& data);

            Shape shape() const;

            Count nr_elements() const;


            HPX_DEFINE_COMPONENT_ACTION(ArrayPartition, data, DataAction)
            HPX_DEFINE_COMPONENT_ACTION(ArrayPartition, slice, SliceAction)
            HPX_DEFINE_COMPONENT_ACTION(ArrayPartition, fill, FillAction)
            HPX_DEFINE_COMPONENT_ACTION(ArrayPartition, set_data, SetDataAction)
            HPX_DEFINE_COMPONENT_ACTION(ArrayPartition, offset, OffsetAction)
            HPX_DEFINE_COMPONENT_ACTION(ArrayPartition, shape, ShapeAction)
            HPX_DEFINE_COMPONENT_ACTION(ArrayPartition, nr_elements, NrElementsAction)


        private:

            Offset _offset;

            Data _data;
    };

}  // namespace lue::server


#define LUE_REGISTER_ARRAY_PARTITION_ACTION_DECLARATIONS(Element, rank, unique)                              \
                                                                                                             \
    namespace lue::detail {                                                                                  \
        using ArrayPartitionServer_##unique = lue::server::ArrayPartition<Element, rank>;                    \
    }                                                                                                        \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        lue::detail::ArrayPartitionServer_##unique::DataAction, ArrayPartitionServerDataAction_##unique)     \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        lue::detail::ArrayPartitionServer_##unique::SliceAction, ArrayPartitionServerSliceAction_##unique)   \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        lue::detail::ArrayPartitionServer_##unique::FillAction, ArrayPartitionServerFillAction_##unique)     \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        lue::detail::ArrayPartitionServer_##unique::SetDataAction,                                           \
        ArrayPartitionServerSetDataAction_##unique)                                                          \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        lue::detail::ArrayPartitionServer_##unique::OffsetAction, ArrayPartitionServerOffsetAction_##unique) \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        lue::detail::ArrayPartitionServer_##unique::ShapeAction, ArrayPartitionServerShapeAction_##unique)   \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        lue::detail::ArrayPartitionServer_##unique::NrElementsAction,                                        \
        ArrayPartitionServerNrElementsAction_##unique)


#define LUE_REGISTER_ARRAY_PARTITION_ACTIONS(Element, rank, unique)                                          \
                                                                                                             \
    namespace lue::detail {                                                                                  \
        using ArrayPartitionServerComponent_##unique =                                                       \
            hpx::components::component<lue::detail::ArrayPartitionServer_##unique>;                          \
    }                                                                                                        \
                                                                                                             \
    HPX_REGISTER_COMPONENT(                                                                                  \
        lue::detail::ArrayPartitionServerComponent_##unique, lue_ArrayPartitionServer_##unique)              \
                                                                                                             \
    HPX_REGISTER_ACTION(                                                                                     \
        lue::detail::ArrayPartitionServer_##unique::DataAction, ArrayPartitionServerDataAction_##unique)     \
                                                                                                             \
    HPX_REGISTER_ACTION(                                                                                     \
        lue::detail::ArrayPartitionServer_##unique::SliceAction, ArrayPartitionServerSliceAction_##unique)   \
                                                                                                             \
    HPX_REGISTER_ACTION(                                                                                     \
        lue::detail::ArrayPartitionServer_##unique::FillAction, ArrayPartitionServerFillAction_##unique)     \
                                                                                                             \
    HPX_REGISTER_ACTION(                                                                                     \
        lue::detail::ArrayPartitionServer_##unique::SetDataAction,                                           \
        ArrayPartitionServerSetDataAction_##unique)                                                          \
                                                                                                             \
    HPX_REGISTER_ACTION(                                                                                     \
        lue::detail::ArrayPartitionServer_##unique::OffsetAction, ArrayPartitionServerOffsetAction_##unique) \
                                                                                                             \
    HPX_REGISTER_ACTION(                                                                                     \
        lue::detail::ArrayPartitionServer_##unique::ShapeAction, ArrayPartitionServerShapeAction_##unique)   \
                                                                                                             \
    HPX_REGISTER_ACTION(                                                                                     \
        lue::detail::ArrayPartitionServer_##unique::NrElementsAction,                                        \
        ArrayPartitionServerNrElementsAction_##unique)


#define LUE_REGISTER_ARRAY_PARTITION_ACTION_DECLARATIONS_RANK(Element)                                       \
    LUE_REGISTER_ARRAY_PARTITION_ACTION_DECLARATIONS(Element, 0, Element##_0)                                \
    LUE_REGISTER_ARRAY_PARTITION_ACTION_DECLARATIONS(Element, 1, Element##_1)                                \
    LUE_REGISTER_ARRAY_PARTITION_ACTION_DECLARATIONS(Element, 2, Element##_2)

#define LUE_REGISTER_ARRAY_PARTITION_ACTION_DECLARATIONS_ELEMENT()                                           \
    LUE_REGISTER_ARRAY_PARTITION_ACTION_DECLARATIONS_RANK(bool)                                              \
    LUE_REGISTER_ARRAY_PARTITION_ACTION_DECLARATIONS_RANK(uint8_t)                                           \
    LUE_REGISTER_ARRAY_PARTITION_ACTION_DECLARATIONS_RANK(int32_t)                                           \
    LUE_REGISTER_ARRAY_PARTITION_ACTION_DECLARATIONS_RANK(uint32_t)                                          \
    LUE_REGISTER_ARRAY_PARTITION_ACTION_DECLARATIONS_RANK(int64_t)                                           \
    LUE_REGISTER_ARRAY_PARTITION_ACTION_DECLARATIONS_RANK(uint64_t)                                          \
    LUE_REGISTER_ARRAY_PARTITION_ACTION_DECLARATIONS_RANK(float)                                             \
    LUE_REGISTER_ARRAY_PARTITION_ACTION_DECLARATIONS_RANK(double)

LUE_REGISTER_ARRAY_PARTITION_ACTION_DECLARATIONS_ELEMENT()

#undef LUE_REGISTER_ARRAY_PARTITION_ACTION_DECLARATIONS
#undef LUE_REGISTER_ARRAY_PARTITION_ACTION_DECLARATIONS_ELEMENT
#undef LUE_REGISTER_ARRAY_PARTITION_ACTION_DECLARATIONS_RANK
