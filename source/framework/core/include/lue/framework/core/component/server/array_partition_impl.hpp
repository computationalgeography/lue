#pragma once
#include "lue/framework/core/component/server/array_partition.hpp"


namespace lue::server {

    /*!
        @brief      Construct an instance based on an array partition @a
                    shape

        The @a shape passed in will be passed to the constructor of @a Data.
    */
    template<
        typename Element,
        Rank rank>
    ArrayPartition<Element, rank>::ArrayPartition(
        Offset const& offset,
        Shape const& shape):

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
    template<
        typename Element,
        Rank rank>
    ArrayPartition<Element, rank>::ArrayPartition(
        Offset const& offset,
        Shape const& shape,
        Element value):

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
    template<
        typename Element,
        Rank rank>
    ArrayPartition<Element, rank>::ArrayPartition(
        Offset const& offset,
        Data const& data):

        Base{},
        _offset{offset},
        _data{data}

    {
    }


    /*!
        @brief      Construct an instance based on initial partition @a data

        The @a data passed in is moved from.
    */
    template<
        typename Element,
        Rank rank>
    ArrayPartition<Element, rank>::ArrayPartition(
        Offset const& offset,
        Data&& data):

        Base{},
        _offset{offset},
        _data{std::move(data)}

    {
    }


    template<
        typename Element,
        Rank rank>
    ArrayPartition<Element, rank>::ArrayPartition(
        ArrayPartition const& other):

        Base{other},
        _offset{other._offset},
        _data{other._data}

    {
    }


    template<
        typename Element,
        Rank rank>
    ArrayPartition<Element, rank>::ArrayPartition(
        ArrayPartition&& other):

        Base{std::move(other)},
        _offset{std::move(other._offset)},
        _data{std::move(other._data)}

    {
    }


    /*!
        @brief      Return this partition's data
    */
    template<
        typename Element,
        Rank rank>
    typename ArrayPartition<Element, rank>::Data
        ArrayPartition<Element, rank>::data() const
    {
        return _data;
    }


    template<
        typename Element,
        Rank rank>
    typename ArrayPartition<Element, rank>::Data
        ArrayPartition<Element, rank>::slice(
            Slices const& slices) const
    {
        if constexpr (rank == 0) {
            return _data;
        }
        else {
            return _data.slice(slices);
        }
    }


    template<
        typename Element,
        Rank rank>
    void ArrayPartition<Element, rank>::fill(
        Element value)
    {
        std::fill(_data.begin(), _data.end(), value);
    }


    template<
        typename Element,
        Rank rank>
    void ArrayPartition<Element, rank>::set_data(
        Data const& data)
    {
        _data = data;
    }


    template<
        typename Element,
        Rank rank>
    typename ArrayPartition<Element, rank>::Offset
        ArrayPartition<Element, rank>::offset() const
    {
        return _offset;
    }


    template<
        typename Element,
        Rank rank>
    typename ArrayPartition<Element, rank>::Shape
        ArrayPartition<Element, rank>::shape() const
    {
        return _data.shape();
    }


    template<
        typename Element,
        Rank rank>
    Count ArrayPartition<Element, rank>::nr_elements() const
    {
        return _data.nr_elements();
    }

}  // namespace lue::server


#define LUE_REGISTER_ARRAY_PARTITION_ACTION_DECLARATIONS(             \
    Element,                                                          \
    rank)                                                             \
                                                                      \
using ArrayPartition_##Element##_##rank##_Component =                 \
    lue::server::ArrayPartition<Element, rank>;                       \
                                                                      \
HPX_REGISTER_ACTION_DECLARATION(                                      \
    ArrayPartition_##Element##_##rank##_Component::DataAction,        \
    ArrayPartition_##Element##_##rank##_DataAction)                   \
                                                                      \
HPX_REGISTER_ACTION_DECLARATION(                                      \
    ArrayPartition_##Element##_##rank##_Component::SliceAction,       \
    ArrayPartition_##Element##_##rank##_SliceAction)                  \
                                                                      \
HPX_REGISTER_ACTION_DECLARATION(                                      \
    ArrayPartition_##Element##_##rank##_Component::FillAction,        \
    ArrayPartition_##Element##_##rank##_FillAction)                   \
                                                                      \
HPX_REGISTER_ACTION_DECLARATION(                                      \
    ArrayPartition_##Element##_##rank##_Component::SetDataAction,     \
    ArrayPartition_##Element##_##rank##_SetDataAction)                \
                                                                      \
HPX_REGISTER_ACTION_DECLARATION(                                      \
    ArrayPartition_##Element##_##rank##_Component::OffsetAction,      \
    ArrayPartition_##Element##_##rank##_OffsetAction)                 \
                                                                      \
HPX_REGISTER_ACTION_DECLARATION(                                      \
    ArrayPartition_##Element##_##rank##_Component::ShapeAction,       \
    ArrayPartition_##Element##_##rank##_ShapeAction)                  \
                                                                      \
HPX_REGISTER_ACTION_DECLARATION(                                      \
    ArrayPartition_##Element##_##rank##_Component::NrElementsAction,  \
    ArrayPartition_##Element##_##rank##_NrElementsAction)


#define LUE_REGISTER_ARRAY_PARTITIONS_ACTION_DECLARATIONS(    \
    Element)                                                  \
                                                              \
LUE_REGISTER_ARRAY_PARTITION_ACTION_DECLARATIONS(Element, 0)  \
LUE_REGISTER_ARRAY_PARTITION_ACTION_DECLARATIONS(Element, 1)  \
LUE_REGISTER_ARRAY_PARTITION_ACTION_DECLARATIONS(Element, 2)


LUE_REGISTER_ARRAY_PARTITIONS_ACTION_DECLARATIONS(bool)
LUE_REGISTER_ARRAY_PARTITIONS_ACTION_DECLARATIONS(/* std:: */ uint8_t)
LUE_REGISTER_ARRAY_PARTITIONS_ACTION_DECLARATIONS(/* std:: */ uint32_t)
LUE_REGISTER_ARRAY_PARTITIONS_ACTION_DECLARATIONS(/* std:: */ uint64_t)
LUE_REGISTER_ARRAY_PARTITIONS_ACTION_DECLARATIONS(/* std:: */ int32_t)
LUE_REGISTER_ARRAY_PARTITIONS_ACTION_DECLARATIONS(/* std:: */ int64_t)
LUE_REGISTER_ARRAY_PARTITIONS_ACTION_DECLARATIONS(float)
LUE_REGISTER_ARRAY_PARTITIONS_ACTION_DECLARATIONS(double)


#undef LUE_REGISTER_ARRAY_PARTITIONS_ACTION_DECLARATIONS
#undef LUE_REGISTER_ARRAY_PARTITION_ACTION_DECLARATIONS
