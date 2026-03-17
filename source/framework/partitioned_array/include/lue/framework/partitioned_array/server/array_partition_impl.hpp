#pragma once
#include "lue/framework/partitioned_array/server/array_partition_decl.hpp"


namespace lue::server {

    /*!
        @brief      Construct an instance based on an array partition @a
                    shape

        The @a shape passed in will be passed to the constructor of @a Data.
    */
    template<typename Element, Rank rank>
    ArrayPartition<Element, rank>::ArrayPartition(Offset const& offset, Shape const& shape):

        Base{},
        _offset{offset},
        _data{shape}

    {
    }


    /*!
        @brief      Construct an instance based on an array partition @a
                    shape and an initial @a value

        The @a shape and the @a value passed in will be passed to the
        constructor of @a Data.
    */
    template<typename Element, Rank rank>
    ArrayPartition<Element, rank>::ArrayPartition(Offset const& offset, Shape const& shape, Element value):

        Base{},
        _offset{offset},
        _data{shape, value}

    {
        // Element is assumed to be a trivial type. Otherwise, don't pass
        // in value by value.
        static_assert(std::is_trivial_v<Element>);
    }


    /*!
        @brief      Construct an instance based on initial partition @a data

        The @a data passed in is copied.
    */
    template<typename Element, Rank rank>
    ArrayPartition<Element, rank>::ArrayPartition(Offset const& offset, Data const& data):

        Base{},
        _offset{offset},
        _data{data}

    {
    }


    /*!
        @brief      Construct an instance based on initial partition @a data

        The @a data passed in is moved from.
    */
    template<typename Element, Rank rank>
    ArrayPartition<Element, rank>::ArrayPartition(Offset const& offset, Data&& data):

        Base{},
        _offset{offset},
        _data{std::move(data)}

    {
    }


    template<typename Element, Rank rank>
    ArrayPartition<Element, rank>::ArrayPartition(ArrayPartition const& other):

        Base{other},
        _offset{other._offset},
        _data{other._data}

    {
    }


    template<typename Element, Rank rank>
    ArrayPartition<Element, rank>::ArrayPartition(ArrayPartition&& other) noexcept:

        Base{std::move(other)},
        _offset{std::move(other._offset)},
        _data{std::move(other._data)}

    {
    }


    /*!
        @brief      Return this partition's data
    */
    template<typename Element, Rank rank>
    auto ArrayPartition<Element, rank>::data() const -> typename ArrayPartition<Element, rank>::Data
    {
        return _data;
    }


    template<typename Element, Rank rank>
    auto ArrayPartition<Element, rank>::slice(Slices const& slices) const ->
        typename ArrayPartition<Element, rank>::Data
    {
        if constexpr (rank == 0)
        {
            return _data;
        }
        else
        {
            return _data.slice(slices);
        }
    }


    template<typename Element, Rank rank>
    void ArrayPartition<Element, rank>::fill(Element value)
    {
        std::fill(_data.begin(), _data.end(), value);
    }


    template<typename Element, Rank rank>
    void ArrayPartition<Element, rank>::set_data(Data const& data)
    {
        _data = data;
    }


    template<typename Element, Rank rank>
    auto ArrayPartition<Element, rank>::offset() const -> typename ArrayPartition<Element, rank>::Offset
    {
        return _offset;
    }


    template<typename Element, Rank rank>
    auto ArrayPartition<Element, rank>::shape() const -> typename ArrayPartition<Element, rank>::Shape
    {
        return _data.shape();
    }


    template<typename Element, Rank rank>
    auto ArrayPartition<Element, rank>::nr_elements() const -> Count
    {
        return _data.nr_elements();
    }

}  // namespace lue::server


// Register array partition action declarations for an array partition type
#define LUE_REGISTER_ARRAY_PARTITION_ACTIONS(ArrayPartition)                                                 \
    HPX_REGISTER_ACTION(ArrayPartition::DataAction, HPX_PP_CAT(ArrayPartition, DataAction));                 \
    HPX_REGISTER_ACTION(ArrayPartition::SliceAction, HPX_PP_CAT(ArrayPartition, SliceAction));               \
    HPX_REGISTER_ACTION(ArrayPartition::FillAction, HPX_PP_CAT(ArrayPartition, FillAction));                 \
    HPX_REGISTER_ACTION(ArrayPartition::SetDataAction, HPX_PP_CAT(ArrayPartition, SetDataAction));           \
    HPX_REGISTER_ACTION(ArrayPartition::OffsetAction, HPX_PP_CAT(ArrayPartition, OffsetAction));             \
    HPX_REGISTER_ACTION(ArrayPartition::ShapeAction, HPX_PP_CAT(ArrayPartition, ShapeAction));               \
    HPX_REGISTER_ACTION(ArrayPartition::NrElementsAction, HPX_PP_CAT(ArrayPartition, NrElementsAction));

#define LUE_REGISTER_ARRAY_PARTITION_COMPONENT(ArrayPartition)                                               \
    using HPX_PP_CAT(ArrayPartition, Component) = hpx::components::component<ArrayPartition>;                \
    HPX_REGISTER_COMPONENT(HPX_PP_CAT(ArrayPartition, Component));

// Register array partition declaration for an element type and rank
#define LUE_REGISTER_ARRAY_PARTITION(Element, rank)                                                          \
    using HPX_PP_CAT(ArrayPartitionServer_, HPX_PP_CAT(Element, rank)) =                                     \
        lue::server::ArrayPartition<Element, rank>;                                                          \
                                                                                                             \
    LUE_REGISTER_ARRAY_PARTITION_COMPONENT(HPX_PP_CAT(ArrayPartitionServer_, HPX_PP_CAT(Element, rank)));    \
    LUE_REGISTER_ARRAY_PARTITION_ACTIONS(HPX_PP_CAT(ArrayPartitionServer_, HPX_PP_CAT(Element, rank)));
