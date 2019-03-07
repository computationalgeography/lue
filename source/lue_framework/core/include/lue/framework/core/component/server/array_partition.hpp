#pragma once
#include "lue/framework/core/array_partition_data.hpp"
#include <hpx/config.hpp>
#include <hpx/include/components.hpp>


namespace lue {
namespace server {

/*!
    @brief      Component server class for partioned array partitions
    @tparam     Data Class representing the array data of a single partition
*/
template<
    typename Value,
    typename Data>
class ArrayPartition:
    public hpx::components::locking_hook<
        hpx::components::component_base<ArrayPartition<Value, Data>>>
{

private:

    static_assert(std::is_same_v<Value, typename Data::ValueType>);

    using Base =
        hpx::components::locking_hook<
            hpx::components::component_base<ArrayPartition<Value, Data>>>;

public:

    using Shape = typename Data::Shape;

    using SizeType = typename Data::SizeType;

                   ArrayPartition      ();

    explicit       ArrayPartition      (Shape const& shape);

                   ArrayPartition      (Shape const& shape,
                                        Value value);

                   ArrayPartition      (Data const& data);

                   ArrayPartition      (ArrayPartition const& other);

                   ArrayPartition      (ArrayPartition&& other);

                   ~ArrayPartition     ()=default;

    ArrayPartition& operator=          (ArrayPartition const&)=delete;

    ArrayPartition& operator=          (ArrayPartition&&)=delete;

    Data           data                () const;

    void           fill                (Value value);

    Shape          shape               () const;

    SizeType       size                () const;

private:

    //! Data of this array partition
    Data           _data;

public:

    // Macros to define HPX component actions for all exported functions
    HPX_DEFINE_COMPONENT_DIRECT_ACTION(ArrayPartition, data, DataAction);
    HPX_DEFINE_COMPONENT_DIRECT_ACTION(ArrayPartition, fill, FillAction);
    HPX_DEFINE_COMPONENT_DIRECT_ACTION(ArrayPartition, shape, ShapeAction);
    HPX_DEFINE_COMPONENT_DIRECT_ACTION(ArrayPartition, size, SizeAction);

};

}  // namespace server
}  // namespace lue


// HPX_REGISTER_ACTION_DECLARATION calls ---------------------------------------

/*
// Forward
#define LUE_REGISTER_ARRAY_PARTITION_DECLARATION(...)       \
    LUE_REGISTER_ARRAY_PARTITION_DECLARATION_(__VA_ARGS__)

// Forward
#define LUE_REGISTER_ARRAY_PARTITION_DECLARATION_(...)  \
    HPX_PP_EXPAND(                                      \
        HPX_PP_CAT(                                     \
            LUE_REGISTER_ARRAY_PARTITION_DECLARATION_,  \
            HPX_PP_NARGS(__VA_ARGS__))(                 \
                __VA_ARGS__))

// Implementation of LUE_REGISTER_ARRAY_PARTITION_DECLARATION macro
// for a array partition type
#define LUE_REGISTER_ARRAY_PARTITION_DECLARATION_IMPL(Type, name)
//     HPX_REGISTER_ACTION_DECLARATION(Type::size_action,             \
//             HPX_PP_CAT(__ArrayPartition_size_action, name));

// Forward
#define LUE_REGISTER_ARRAY_PARTITION_DECLARATION_2(Value, rank)  \
    LUE_REGISTER_ARRAY_PARTITION_DECLARATION_3(                  \
        Value, rank,                                             \
        ArrayPartitionData<Value, rank>)

// Forward
#define LUE_REGISTER_ARRAY_PARTITION_DECLARATION_3(Value, rank, Data)  \
    static_assert(rank == Data::rank);                                 \
    LUE_REGISTER_ARRAY_PARTITION_DECLARATION_4(                        \
        Value, rank,                                                   \
        Data,                                                          \
        Value)

// Call implementation
#define LUE_REGISTER_ARRAY_PARTITION_DECLARATION_4(Value, Data, rank, name)  \
    using HPX_PP_CAT(                                                        \
            __ArrayPartition_,                                               \
            HPX_PP_CAT(HPX_PP_CAT(Value, rank), name)) =                     \
                ::lue::server::ArrayPartition<Value, Data>;                  \
    LUE_REGISTER_ARRAY_PARTITION_DECLARATION_IMPL(                           \
        HPX_PP_CAT(                                                          \
            __ArrayPartition_,                                               \
            HPX_PP_CAT(HPX_PP_CAT(Value, rank), name)), name)
*/


// HPX_REGISTER_ACTION calls and HPX_REGISTER_COMPONENT call -------------------

// Forward
#define LUE_REGISTER_ARRAY_PARTITION(...)       \
    LUE_REGISTER_ARRAY_PARTITION_(__VA_ARGS__)

// Forward
#define LUE_REGISTER_ARRAY_PARTITION_(...)  \
    HPX_PP_EXPAND(                          \
        HPX_PP_CAT(                         \
            LUE_REGISTER_ARRAY_PARTITION_,  \
            HPX_PP_NARGS(__VA_ARGS__)       \
        )(__VA_ARGS__)                      \
    )

// Implementation of LUE_REGISTER_ARRAY_PARTITION macro
// for an array partition type
#define LUE_REGISTER_ARRAY_PARTITION_IMPL(Server, name)   \
    HPX_REGISTER_ACTION(Server::DataAction,               \
        HPX_PP_CAT(__ArrayPartition_DataAction_, name))   \
    HPX_REGISTER_ACTION(Server::FillAction,               \
        HPX_PP_CAT(__ArrayPartition_FillAction_, name))   \
    HPX_REGISTER_ACTION(Server::ShapeAction,              \
        HPX_PP_CAT(__ArrayPartition_shape_action_, name))  \
    HPX_REGISTER_ACTION(Server::SizeAction,               \
        HPX_PP_CAT(__ArrayPartition_size_action_, name))  \
    using HPX_PP_CAT(__ArrayPartition_, name) =           \
        ::hpx::components::component<Server>;             \
    HPX_REGISTER_COMPONENT(HPX_PP_CAT(__ArrayPartition_, name))

// Forward
#define LUE_REGISTER_ARRAY_PARTITION_2(Value, rank)                     \
    using HPX_PP_CAT(HPX_PP_CAT(HPX_PP_CAT(DefaultData_, Value), _), rank) = \
        ::lue::ArrayPartitionData<Value, rank>;         \
    LUE_REGISTER_ARRAY_PARTITION_4(                                     \
        Value, rank,                                                    \
        HPX_PP_CAT(HPX_PP_CAT(HPX_PP_CAT(DefaultData_, Value), _), rank), \
        HPX_PP_CAT(  \
            HPX_PP_CAT(HPX_PP_CAT(lue_ArrayPartitionData_, Value), _), rank))

// Forward
#define LUE_REGISTER_ARRAY_PARTITION_3(Value, rank, Data)  \
    static_assert(rank == Data::rank);                     \
    LUE_REGISTER_ARRAY_PARTITION_4(                        \
        Value, rank,                                       \
        Data,                                              \
        HPX_PP_CAT(HPX_PP_CAT(Data, Value)_, rank))

// Call implementation
// Value: Value type of array elements
// Data: Array type for storing array elements
// name: Label for Data. This label will be uniqified using Value and rank
#define LUE_REGISTER_ARRAY_PARTITION_4(Value, rank, Data, name)  \
    using HPX_PP_CAT(                                            \
            __ArrayPartition_,                                   \
            HPX_PP_CAT(HPX_PP_CAT(Value, _), name)) =            \
        ::lue::server::ArrayPartition<Value, Data>;              \
    LUE_REGISTER_ARRAY_PARTITION_IMPL(                           \
        HPX_PP_CAT(                                              \
            __ArrayPartition_,                                   \
            HPX_PP_CAT(HPX_PP_CAT(Value, _), name)), name)


namespace lue {
namespace server {

template<
    typename Value,
    typename Data>
ArrayPartition<Value, Data>::ArrayPartition():

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
    typename Value,
    typename Data>
ArrayPartition<Value, Data>::ArrayPartition(
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
    typename Value,
    typename Data>
ArrayPartition<Value, Data>::ArrayPartition(
    Shape const& shape,
    Value value):

    Base{},
    _data{shape, value}

{
    // Value is assumed to be a trivial type. Otherwise, don't pass
    // in value by value.
    static_assert(std::is_trivial_v<Value>);
}


/*!
    @brief      Construct an instance based on initial partition @a data
*/
template<
    typename Value,
    typename Data>
ArrayPartition<Value, Data>::ArrayPartition(
    Data const& data):

    Base{},
    _data{data}

{
}


template<
    typename Value,
    typename Data>
ArrayPartition<Value, Data>::ArrayPartition(
    ArrayPartition const& other):

    Base{other},
    _data{other._data}

{
}


template<
    typename Value,
    typename Data>
ArrayPartition<Value, Data>::ArrayPartition(
    ArrayPartition&& other):

    Base{std::move(other)},
    _data{std::move(other._data)}

{
}


/*!
    @brief      Return this partition's data
*/
template<
    typename Value,
    typename Data>
Data ArrayPartition<Value, Data>::data() const
{
    return _data;
}


template<
    typename Value,
    typename Data>
void ArrayPartition<Value, Data>::fill(
    Value value)
{
    std::fill(_data.data(), _data.data() + _data.size(), value);
}


template<
    typename Value,
    typename Data>
typename ArrayPartition<Value, Data>::Shape
    ArrayPartition<Value, Data>::shape() const
{
    return _data.shape();
}


template<
    typename Value,
    typename Data>
typename ArrayPartition<Value, Data>::SizeType
    ArrayPartition<Value, Data>::size() const
{
    return _data.size();
}

}  // namespace server
}  // namespace lue
