#pragma once
#include <hpx/config.hpp>
#include "lue/framework/core/array_partition_data.hpp"
#include "lue/framework/core/offset.hpp"
#include <hpx/include/components.hpp>


namespace lue {
namespace server {

/*!
    @brief      Component server class for partioned array partitions
*/
template<
    typename Element,
    Rank rank>
class ArrayPartition:
    public hpx::components::locking_hook<
        hpx::components::component_base<ArrayPartition<Element, rank>>>
{

private:

    using Base =
        hpx::components::locking_hook<
            hpx::components::component_base<ArrayPartition<Element, rank>>>;

public:

    using Data = ArrayPartitionData<Element, rank>;

    using Shape = typename Data::Shape;

    using Offset = lue::Offset<Index, rank>;

    using Slice = typename Data::Slice;

    using Slices = typename Data::Slices;

    ///                ArrayPartition      ();

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

    // hpx::naming::id_type locality_id() const
    // {
    //     lue_assert(_locality_id);

    //     return _locality_id;
    // }

    Offset         offset              () const;

    Data           data                () const;

    Data           slice               ([[maybe_unused]] Slices const& slices) const;

    void           fill                (Element value);

    void           set_data            (Data const& data);

    Shape          shape               () const;

    void           reshape             (Shape const& shape);

    Count          nr_elements         () const;

private:

    // //! Global ID representing the locality this component is located in
    // hpx::naming::id_type _locality_id;

    Offset         _offset;

    //! Data of this array partition
    Data           _data;

public:

    // Macros to define HPX component actions for all exported functions
    HPX_DEFINE_COMPONENT_ACTION(ArrayPartition, data, DataAction);
    HPX_DEFINE_COMPONENT_ACTION(ArrayPartition, slice, SliceAction);
    HPX_DEFINE_COMPONENT_ACTION(ArrayPartition, fill, FillAction);
    HPX_DEFINE_COMPONENT_ACTION(ArrayPartition, set_data, SetDataAction);
    HPX_DEFINE_COMPONENT_ACTION(ArrayPartition, offset, OffsetAction);
    HPX_DEFINE_COMPONENT_ACTION(ArrayPartition, shape, ShapeAction);
    HPX_DEFINE_COMPONENT_ACTION(ArrayPartition, reshape, ReshapeAction);
    HPX_DEFINE_COMPONENT_ACTION(ArrayPartition, nr_elements, NrElementsAction);

};

}  // namespace server
}  // namespace lue


#define LUE_REGISTER_ARRAY_PARTITION_ACTION_DECLARATIONS(              \
    Element,                                                           \
    rank)                                                              \
                                                                       \
namespace lue {                                                        \
namespace detail {                                                     \
                                                                       \
using ArrayPartition_##Element##_##rank =                              \
    server::ArrayPartition<Element, rank>;                             \
                                                                       \
}                                                                      \
}                                                                      \
                                                                       \
HPX_REGISTER_ACTION_DECLARATION(                                       \
    lue::detail::ArrayPartition_##Element##_##rank::DataAction,        \
    ArrayPartition_##Element##_##rank##_DataAction)                    \
                                                                       \
HPX_REGISTER_ACTION_DECLARATION(                                       \
    lue::detail::ArrayPartition_##Element##_##rank::SliceAction,       \
    ArrayPartition_##Element##_##rank##_SliceAction)                   \
                                                                       \
HPX_REGISTER_ACTION_DECLARATION(                                       \
    lue::detail::ArrayPartition_##Element##_##rank::FillAction,        \
    ArrayPartition_##Element##_##rank##_FillAction)                    \
                                                                       \
HPX_REGISTER_ACTION_DECLARATION(                                       \
    lue::detail::ArrayPartition_##Element##_##rank::SetDataAction,     \
    ArrayPartition_##Element##_##rank##_SetDataAction)                 \
                                                                       \
HPX_REGISTER_ACTION_DECLARATION(                                       \
    lue::detail::ArrayPartition_##Element##_##rank::OffsetAction,      \
    ArrayPartition_##Element##_##rank##_OffsetAction)                  \
                                                                       \
HPX_REGISTER_ACTION_DECLARATION(                                       \
    lue::detail::ArrayPartition_##Element##_##rank::ShapeAction,       \
    ArrayPartition_##Element##_##rank##_ShapeAction)                   \
                                                                       \
HPX_REGISTER_ACTION_DECLARATION(                                       \
    lue::detail::ArrayPartition_##Element##_##rank::ReshapeAction,     \
    ArrayPartition_##Element##_##rank##_ReshapeAction)                 \
                                                                       \
HPX_REGISTER_ACTION_DECLARATION(                                       \
    lue::detail::ArrayPartition_##Element##_##rank::NrElementsAction,  \
    ArrayPartition_##Element##_##rank##_NrElementsAction)

#define LUE_REGISTER_ARRAY_PARTITIONS_ACTION_DECLARATIONS(    \
    Element)                                                  \
                                                              \
LUE_REGISTER_ARRAY_PARTITION_ACTION_DECLARATIONS(Element, 0)  \
LUE_REGISTER_ARRAY_PARTITION_ACTION_DECLARATIONS(Element, 1)  \
LUE_REGISTER_ARRAY_PARTITION_ACTION_DECLARATIONS(Element, 2)

LUE_REGISTER_ARRAY_PARTITIONS_ACTION_DECLARATIONS(bool)
LUE_REGISTER_ARRAY_PARTITIONS_ACTION_DECLARATIONS(/* std:: */ uint32_t)
LUE_REGISTER_ARRAY_PARTITIONS_ACTION_DECLARATIONS(/* std:: */ uint64_t)
LUE_REGISTER_ARRAY_PARTITIONS_ACTION_DECLARATIONS(/* std:: */ int32_t)
LUE_REGISTER_ARRAY_PARTITIONS_ACTION_DECLARATIONS(/* std:: */ int64_t)
LUE_REGISTER_ARRAY_PARTITIONS_ACTION_DECLARATIONS(float)
LUE_REGISTER_ARRAY_PARTITIONS_ACTION_DECLARATIONS(double)

#undef LUE_REGISTER_ARRAY_PARTITION_ACTION_DECLARATIONS
#undef LUE_REGISTER_ARRAY_PARTITIONS_ACTION_DECLARATIONS


#define LUE_DEFINE_ARRAY_PARTITION_COMPONENT_ACTION_TEMPLATE(          \
    name)                                                              \
                                                                       \
namespace lue {                                                        \
                                                                       \
template<                                                              \
    typename Element,                                                  \
    Rank rank>                                                         \
class ArrayPartition##name##Action                                     \
{                                                                      \
                                                                       \
public:                                                                \
                                                                       \
    using Type =                                                       \
        typename server::ArrayPartition<Element, rank>::name##Action;  \
                                                                       \
};                                                                     \
                                                                       \
}

LUE_DEFINE_ARRAY_PARTITION_COMPONENT_ACTION_TEMPLATE(Data)
LUE_DEFINE_ARRAY_PARTITION_COMPONENT_ACTION_TEMPLATE(Slice)
LUE_DEFINE_ARRAY_PARTITION_COMPONENT_ACTION_TEMPLATE(Fill)
LUE_DEFINE_ARRAY_PARTITION_COMPONENT_ACTION_TEMPLATE(SetData)
LUE_DEFINE_ARRAY_PARTITION_COMPONENT_ACTION_TEMPLATE(Offset)
LUE_DEFINE_ARRAY_PARTITION_COMPONENT_ACTION_TEMPLATE(Shape)
LUE_DEFINE_ARRAY_PARTITION_COMPONENT_ACTION_TEMPLATE(Reshape)
LUE_DEFINE_ARRAY_PARTITION_COMPONENT_ACTION_TEMPLATE(NrElements)

#undef LUE_DEFINE_ARRAY_PARTITION_COMPONENT_ACTION_TEMPLATE


namespace lue {
namespace server {

// template<
//     typename Element,
//     Rank rank>
// ArrayPartition<Element, rank>::ArrayPartition():
// 
//     Base{},
//     _locality_id{hpx::find_here()},
//     _offset{},
//     _data{}
// 
// {
//     HPX_ASSERT(false);
// }


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
    // _locality_id{hpx::find_here()},
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
    // _locality_id{hpx::find_here()},
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
    // _locality_id{hpx::find_here()},
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
    // _locality_id{hpx::find_here()},
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
    // _locality_id{hpx::find_here()},
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
    // _locality_id{hpx::find_here()},
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

    // if constexpr (rank == 0) {
    //     return _data;
    // }
    // else {
    //     // The elements pointed to by the new data instance will be
    //     // located at the same target as the source instance
    //     return Data{_data};
    // }
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

    // if constexpr (rank == 0) {
    //     _data = data;
    // }
    // else {
    //     // The elements pointed to by the updated data instance will be
    //     // located at the same target as the source instance
    //     _data = Data{data, mode};
    // }
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
void ArrayPartition<Element, rank>::reshape(
    Shape const& shape)
{
    _data.reshape(shape);
}


template<
    typename Element,
    Rank rank>
Count ArrayPartition<Element, rank>::nr_elements() const
{
    return _data.nr_elements();
}

}  // namespace server
}  // namespace lue
