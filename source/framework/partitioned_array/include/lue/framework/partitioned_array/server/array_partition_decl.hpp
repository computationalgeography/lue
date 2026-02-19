#pragma once
#include "lue/framework/core/offset.hpp"
#include "lue/framework/partitioned_array/array_partition_data.hpp"
#include "lue/framework/partitioned_array/export.hpp"
#include <hpx/include/components.hpp>
#include <hpx/preprocessor/cat.hpp>


namespace lue::server {

    /*!
        @brief      Component server class for partitioned array partitions
    */
    template<typename Element, Rank rank>
    class LUE_FPA_EXPORT ArrayPartition:
        public hpx::components::locking_hook<hpx::components::component_base<ArrayPartition<Element, rank>>>
    {

        private:

            using Base =
                hpx::components::locking_hook<hpx::components::component_base<ArrayPartition<Element, rank>>>;

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

            ArrayPartition(ArrayPartition&& other) noexcept;

            ~ArrayPartition() = default;

            auto operator=(ArrayPartition const&) -> ArrayPartition& = delete;

            auto operator=(ArrayPartition&&) -> ArrayPartition& = delete;


            auto offset() const -> typename ArrayPartition<Element, rank>::Offset;

            auto data() const -> typename ArrayPartition<Element, rank>::Data;

            auto slice(Slices const& slices) const -> typename ArrayPartition<Element, rank>::Data;

            void fill(Element value);

            void set_data(Data const& data);

            auto shape() const -> typename ArrayPartition<Element, rank>::Shape;

            auto nr_elements() const -> Count;

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


// Register array partition action declarations for an array partition type
#define LUE_REGISTER_ARRAY_PARTITION_ACTION_DECLARATIONS(ArrayPartition)                                     \
    HPX_REGISTER_ACTION_DECLARATION(ArrayPartition::DataAction, HPX_PP_CAT(ArrayPartition, DataAction))      \
    HPX_REGISTER_ACTION_DECLARATION(ArrayPartition::SliceAction, HPX_PP_CAT(ArrayPartition, SliceAction))    \
    HPX_REGISTER_ACTION_DECLARATION(ArrayPartition::FillAction, HPX_PP_CAT(ArrayPartition, FillAction))      \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        ArrayPartition::SetDataAction, HPX_PP_CAT(ArrayPartition, SetDataAction))                            \
    HPX_REGISTER_ACTION_DECLARATION(ArrayPartition::OffsetAction, HPX_PP_CAT(ArrayPartition, OffsetAction))  \
    HPX_REGISTER_ACTION_DECLARATION(ArrayPartition::ShapeAction, HPX_PP_CAT(ArrayPartition, ShapeAction))    \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        ArrayPartition::NrElementsAction, HPX_PP_CAT(ArrayPartition, NrElementsAction))

// Register array partition declaration for an element type and rank
#define LUE_REGISTER_ARRAY_PARTITION_DECLARATION(Element, rank)                                              \
    using HPX_PP_CAT(ArrayPartitionServer_, HPX_PP_CAT(Element, rank)) =                                     \
        lue::server::ArrayPartition<Element, rank>;                                                          \
                                                                                                             \
    LUE_REGISTER_ARRAY_PARTITION_ACTION_DECLARATIONS(                                                        \
        HPX_PP_CAT(ArrayPartitionServer_, HPX_PP_CAT(Element, rank)));
