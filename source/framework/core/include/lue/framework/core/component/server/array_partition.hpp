#pragma once
#include "lue/framework/core/array_partition_data.hpp"
#include <hpx/config.hpp>
#include <hpx/include/components.hpp>


namespace lue {
namespace server {

/*!
    @brief      Component server class for partioned array partitions
*/
template<
    typename Element,
    std::size_t rank>
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

    using Size = typename Data::Size;

                   ArrayPartition      ();

    explicit       ArrayPartition      (Shape const& shape);

                   ArrayPartition      (Shape const& shape,
                                        Element value);

                   ArrayPartition      (Data const& data);

                   ArrayPartition      (ArrayPartition const& other);

                   ArrayPartition      (ArrayPartition&& other);

                   ~ArrayPartition     ()=default;

    ArrayPartition& operator=          (ArrayPartition const&)=delete;

    ArrayPartition& operator=          (ArrayPartition&&)=delete;

    Data           data                () const;

    void           fill                (Element value);

    Shape          shape               () const;

    Size           size                () const;

private:

    //! Data of this array partition
    Data           _data;

public:

    // Macros to define HPX component actions for all exported functions
    HPX_DEFINE_COMPONENT_ACTION(ArrayPartition, data, DataAction);
    HPX_DEFINE_COMPONENT_ACTION(ArrayPartition, fill, FillAction);
    HPX_DEFINE_COMPONENT_ACTION(ArrayPartition, shape, ShapeAction);
    HPX_DEFINE_COMPONENT_ACTION(ArrayPartition, size, SizeAction);

};

}  // namespace server
}  // namespace lue


#define LUE_REGISTER_ARRAY_PARTITION_ACTION_DECLARATIONS(        \
    Element,                                                     \
    rank)                                                        \
                                                                 \
namespace lue {                                                  \
namespace detail {                                               \
                                                                 \
using ArrayPartition_##Element##_##rank =                        \
    server::ArrayPartition<Element, rank>;                       \
                                                                 \
}                                                                \
}                                                                \
                                                                 \
HPX_REGISTER_ACTION_DECLARATION(                                 \
    lue::detail::ArrayPartition_##Element##_##rank::DataAction,  \
    ArrayPartition_##Element##_##rank##_DataAction)              \
                                                                 \
HPX_REGISTER_ACTION_DECLARATION(                                 \
    lue::detail::ArrayPartition_##Element##_##rank::FillAction,  \
    ArrayPartition_##Element##_##rank##_FillAction)              \
                                                                 \
HPX_REGISTER_ACTION_DECLARATION(                                 \
    lue::detail::ArrayPartition_##Element##_##rank::ShapeAction, \
    ArrayPartition_##Element##_##rank##_ShapeAction)             \
                                                                 \
HPX_REGISTER_ACTION_DECLARATION(                                 \
    lue::detail::ArrayPartition_##Element##_##rank::SizeAction,  \
    ArrayPartition_##Element##_##rank##_SizeAction)


#define LUE_REGISTER_ARRAY_PARTITIONS_ACTION_DECLARATIONS(    \
    Element)                                                  \
                                                              \
LUE_REGISTER_ARRAY_PARTITION_ACTION_DECLARATIONS(Element, 0)  \
LUE_REGISTER_ARRAY_PARTITION_ACTION_DECLARATIONS(Element, 1)  \
LUE_REGISTER_ARRAY_PARTITION_ACTION_DECLARATIONS(Element, 2)

LUE_REGISTER_ARRAY_PARTITIONS_ACTION_DECLARATIONS(/* std:: */ int32_t)
LUE_REGISTER_ARRAY_PARTITIONS_ACTION_DECLARATIONS(/* std:: */ int64_t)
LUE_REGISTER_ARRAY_PARTITIONS_ACTION_DECLARATIONS(float)
LUE_REGISTER_ARRAY_PARTITIONS_ACTION_DECLARATIONS(double)

#undef LUE_REGISTER_ARRAY_PARTITION_ACTION_DECLARATIONS
#undef LUE_REGISTER_ARRAY_PARTITIONS_ACTION_DECLARATIONS


#define LUE_DEFINE_ARRAY_PARTITION_COMPONENT_ACTION_TEMPLATE(          \
        name)                                                          \
                                                                       \
namespace lue {                                                        \
                                                                       \
template<                                                              \
    typename Element,                                                  \
    std::size_t rank>                                                  \
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
LUE_DEFINE_ARRAY_PARTITION_COMPONENT_ACTION_TEMPLATE(Fill)
LUE_DEFINE_ARRAY_PARTITION_COMPONENT_ACTION_TEMPLATE(Shape)
LUE_DEFINE_ARRAY_PARTITION_COMPONENT_ACTION_TEMPLATE(Size)

#undef LUE_DEFINE_ARRAY_PARTITION_COMPONENT_ACTION_TEMPLATE


namespace lue {
namespace server {

template<
    typename Element,
    std::size_t rank>
ArrayPartition<Element, rank>::ArrayPartition():

    Base{},
    _data{}

{
    HPX_ASSERT(false);
}


/*!
    @brief      Construct an instance based on an array partition @a
                shape

    The @a shape passed in will be passed to the constructor of @a Data.
*/
template<
    typename Element,
    std::size_t rank>
ArrayPartition<Element, rank>::ArrayPartition(
    Shape const& shape):

    Base{},
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
    std::size_t rank>
ArrayPartition<Element, rank>::ArrayPartition(
    Shape const& shape,
    Element value):

    Base{},
    _data{shape, value}

{
    // Element is assumed to be a trivial type. Otherwise, don't pass
    // in value by value.
    static_assert(std::is_trivial_v<Element>);
}


/*!
    @brief      Construct an instance based on initial partition @a data
*/
template<
    typename Element,
    std::size_t rank>
ArrayPartition<Element, rank>::ArrayPartition(
    Data const& data):

    Base{},
    _data{data}

{
}


template<
    typename Element,
    std::size_t rank>
ArrayPartition<Element, rank>::ArrayPartition(
    ArrayPartition const& other):

    Base{other},
    _data{other._data}

{
}


template<
    typename Element,
    std::size_t rank>
ArrayPartition<Element, rank>::ArrayPartition(
    ArrayPartition&& other):

    Base{std::move(other)},
    _data{std::move(other._data)}

{
}


/*!
    @brief      Return this partition's data
*/
template<
    typename Element,
    std::size_t rank>
typename ArrayPartition<Element, rank>::Data
    ArrayPartition<Element, rank>::data() const
{
    return _data;
}


template<
    typename Element,
    std::size_t rank>
void ArrayPartition<Element, rank>::fill(
    Element value)
{
    std::fill(_data.data(), _data.data() + _data.size(), value);
}


template<
    typename Element,
    std::size_t rank>
typename ArrayPartition<Element, rank>::Shape
    ArrayPartition<Element, rank>::shape() const
{
    return _data.shape();
}


template<
    typename Element,
    std::size_t rank>
typename ArrayPartition<Element, rank>::Size
    ArrayPartition<Element, rank>::size() const
{
    return _data.size();
}

}  // namespace server
}  // namespace lue
