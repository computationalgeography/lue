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

                   ArrayPartition      ();

    explicit       ArrayPartition      (Shape const& shape);

                   ArrayPartition      (Shape const& shape,
                                        Element value);

                   ArrayPartition      (Data const& data);

                   ArrayPartition      (Data&& data);

                   ArrayPartition      (ArrayPartition const& other);

                   ArrayPartition      (ArrayPartition&& other);

                   ~ArrayPartition     ()=default;

    ArrayPartition& operator=          (ArrayPartition const&)=delete;

    ArrayPartition& operator=          (ArrayPartition&&)=delete;

    Data           data                ([[maybe_unused]] CopyMode mode) const;

    void           fill                (Element value);

    void           set_data            (Data const& data,
                                        [[maybe_unused]] CopyMode mode);

    Shape          shape               () const;

    void           reshape             (Shape const& shape);

    Count          nr_elements         () const;

private:

    //! Data of this array partition
    Data           _data;

public:

    // Macros to define HPX component actions for all exported functions
    HPX_DEFINE_COMPONENT_ACTION(ArrayPartition, data, DataAction);
    HPX_DEFINE_COMPONENT_ACTION(ArrayPartition, fill, FillAction);
    HPX_DEFINE_COMPONENT_ACTION(ArrayPartition, set_data, SetDataAction);
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
    lue::detail::ArrayPartition_##Element##_##rank::FillAction,        \
    ArrayPartition_##Element##_##rank##_FillAction)                    \
                                                                       \
HPX_REGISTER_ACTION_DECLARATION(                                       \
    lue::detail::ArrayPartition_##Element##_##rank::SetDataAction,     \
    ArrayPartition_##Element##_##rank##_SetDataAction)                 \
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
LUE_REGISTER_ARRAY_PARTITIONS_ACTION_DECLARATIONS(/* std:: */ int32_t)
LUE_REGISTER_ARRAY_PARTITIONS_ACTION_DECLARATIONS(/* std:: */ int64_t)
// LUE_REGISTER_ARRAY_PARTITIONS_ACTION_DECLARATIONS(float)
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
LUE_DEFINE_ARRAY_PARTITION_COMPONENT_ACTION_TEMPLATE(Fill)
LUE_DEFINE_ARRAY_PARTITION_COMPONENT_ACTION_TEMPLATE(SetData)
LUE_DEFINE_ARRAY_PARTITION_COMPONENT_ACTION_TEMPLATE(Shape)
LUE_DEFINE_ARRAY_PARTITION_COMPONENT_ACTION_TEMPLATE(Reshape)
LUE_DEFINE_ARRAY_PARTITION_COMPONENT_ACTION_TEMPLATE(NrElements)

#undef LUE_DEFINE_ARRAY_PARTITION_COMPONENT_ACTION_TEMPLATE


namespace lue {
namespace server {

template<
    typename Element,
    Rank rank>
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
    Rank rank>
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
    Rank rank>
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

    The @a data passed in is copied.
*/
template<
    typename Element,
    Rank rank>
ArrayPartition<Element, rank>::ArrayPartition(
    Data const& data):

    Base{},
    _data{data, CopyMode::copy}

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
    Data&& data):

    Base{},
    _data{std::move(data)}

{
}


template<
    typename Element,
    Rank rank>
ArrayPartition<Element, rank>::ArrayPartition(
    ArrayPartition const& other):

    Base{other},
    _data{other._data}

{
}


template<
    typename Element,
    Rank rank>
ArrayPartition<Element, rank>::ArrayPartition(
    ArrayPartition&& other):

    Base{std::move(other)},
    _data{std::move(other._data)}  // ← other is already move from...

{
}


/*!
    @brief      Return this partition's data
*/
template<
    typename Element,
    Rank rank>
typename ArrayPartition<Element, rank>::Data
    ArrayPartition<Element, rank>::data(
        CopyMode const mode) const
{
    if constexpr (rank == 0) {
        return _data;
    }
    else {
        return Data{_data, mode};
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
    Data const& data,
    CopyMode const mode)
{
    if constexpr (rank == 0) {
        _data = data;
    }
    else {
        _data = Data{data, mode};
    }
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
