#pragma once
#include <hpx/config.hpp>
#include "lue/framework/core/array_partition_data.hpp"
#include "lue/framework/core/offset.hpp"
#include <hpx/include/components.hpp>


namespace lue::server {

    /*!
        @brief      Component server class for partioned array partitions
    */
    template<
        typename Element,
        Rank rank>
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


                           ArrayPartition      (Offset const& offset,
                                                Shape const& shape);

                           ArrayPartition      (Offset const& offset,
                                                Shape const& shape,
                                                Element value);

                           ArrayPartition      (Offset const& offset,
                                                Data const& data);

                           ArrayPartition      (Offset const& offset,
                                                Data&& data);

                           ArrayPartition      (ArrayPartition const& other);

                           ArrayPartition      (ArrayPartition&& other);

                           ~ArrayPartition     ()=default;

            ArrayPartition& operator=          (ArrayPartition const&)=delete;

            ArrayPartition& operator=          (ArrayPartition&&)=delete;

            Offset         offset              () const;

            Data           data                () const;

            Data           slice               ([[maybe_unused]] Slices const& slices) const;

            void           fill                (Element value);

            void           set_data            (Data const& data);

            Shape          shape               () const;

            Count          nr_elements         () const;


            HPX_DEFINE_COMPONENT_ACTION(ArrayPartition, data, DataAction);
            HPX_DEFINE_COMPONENT_ACTION(ArrayPartition, slice, SliceAction);
            HPX_DEFINE_COMPONENT_ACTION(ArrayPartition, fill, FillAction);
            HPX_DEFINE_COMPONENT_ACTION(ArrayPartition, set_data, SetDataAction);
            HPX_DEFINE_COMPONENT_ACTION(ArrayPartition, offset, OffsetAction);
            HPX_DEFINE_COMPONENT_ACTION(ArrayPartition, shape, ShapeAction);
            HPX_DEFINE_COMPONENT_ACTION(ArrayPartition, nr_elements, NrElementsAction);


        private:

            Offset _offset;

            Data _data;

        };

}  // namespace lue::server
