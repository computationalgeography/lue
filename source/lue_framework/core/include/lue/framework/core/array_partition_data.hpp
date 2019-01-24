#pragma once
#include "lue/framework/core/array_partition_definition.hpp"
#include <boost/multi_array.hpp>
#include <algorithm>


namespace lue {

template<
    typename Value,
    std::size_t rank_>
class ArrayPartitionData
{

public:

    static const auto rank = rank_;

    using Values = boost::multi_array<Value, rank>;

    using value_type = typename Values::element;
    static_assert(std::is_same_v<value_type, Value>);

    static_assert(std::is_unsigned_v<typename Values::size_type>);
    using size_type = typename Values::size_type;
    static_assert(std::is_unsigned_v<size_type>);

    using index = size_type;  // typename Values::index;
    static_assert(std::is_unsigned_v<index>);

    using Definition = ArrayPartitionDefinition<index, rank>;

                   ArrayPartitionData  ();

                   ArrayPartitionData  (Definition const& definition);

                   ArrayPartitionData  (Definition const& definition,
                                        Value const& value);

                   ArrayPartitionData  (ArrayPartitionData const& other);

                   ArrayPartitionData  (ArrayPartitionData&& other);

                   ~ArrayPartitionData ()=default;

    ArrayPartitionData& operator=      (ArrayPartitionData const& other);

    ArrayPartitionData& operator=      (ArrayPartitionData&& other);

    bool           operator==          (ArrayPartitionData const& other) const;

    Definition const& definition       () const;

    size_type      size                () const;

    Values const&  values              () const;

    Values&        values              ();

    Value*         data                ();

    Value const*   data                () const;

    // Value&         operator[]          (index idx);

    // Value const&   operator[]          (index idx) const;

private:

    Definition     _definition;

    Values         _values;

};


template<
    typename Value,
    std::size_t rank>
ArrayPartitionData<Value, rank>::ArrayPartitionData():

    _definition{},
    _values{}

{
}


template<
    typename Value,
    std::size_t rank>
ArrayPartitionData<Value, rank>::ArrayPartitionData(
    Definition const& definition):

    _definition{definition},
    _values{definition.shape()}

{
}


template<
    typename Value,
    std::size_t rank>
ArrayPartitionData<Value, rank>::ArrayPartitionData(
    Definition const& definition,
    Value const& value):

    _definition{definition},
    _values{definition.shape()}

{
    std::fill_n(_values.data(), _values.num_elements(), value);

    // hpx::parallel::fill_n(
    //     hpx::parallel::execution::par,
    //     _values.data(), _values.num_elements(), value);
}


/*!
    @brief      Copy-construct and instance based on @a other
*/
template<
    typename Value,
    std::size_t rank>
ArrayPartitionData<Value, rank>::ArrayPartitionData(
    ArrayPartitionData const& other):

    _definition{other._definition},
    _values{other._definition.shape()}

{
    auto const src_begin = other._values.data();
    auto const src_end = src_begin + other._values.num_elements();
    auto dst_begin = _values.data();

    std::copy(src_begin, src_end, dst_begin);

    // hpx::parallel::copy(
    //     hpx::parallel::execution::par, src_begin, src_end, dst_begin);
}


/*!
    @brief      Move-construct and instance based on @a other
*/
template<
    typename Value,
    std::size_t rank>
ArrayPartitionData<Value, rank>::ArrayPartitionData(
    ArrayPartitionData&& other):

    _definition{},
    _values{}

{
    *this = std::move(other);
}


/*!
    @brief      Assign @a other to this instance
*/
template<
    typename Value,
    std::size_t rank>
ArrayPartitionData<Value, rank>&
        ArrayPartitionData<Value, rank>::operator=(
    ArrayPartitionData const& other)
{
    _definition = other._definition;
    _values.resize(other._definition.shape());

    auto const src_begin = other._values.data();
    auto const src_end = src_begin + other._values.num_elements();
    auto dst_begin = _values.data();

    std::copy(src_begin, src_end, dst_begin);

    // hpx::parallel::copy(
    //     hpx::parallel::execution::par, src_begin, src_end, dst_begin);

    return *this;
}


/*!
    @brief      Move assign @a other to this instance
*/
template<
    typename Value,
    std::size_t rank>
ArrayPartitionData<Value, rank>&
        ArrayPartitionData<Value, rank>::operator=(
    ArrayPartitionData&& other)
{
    _definition = std::move(other._definition);

    // Copy values and empty the source array afterwards
    _values.resize(_definition.shape());

    auto const src_begin = other._values.data();
    auto const src_end = src_begin + other._values.num_elements();
    auto dst_begin = _values.data();

    std::copy(src_begin, src_end, dst_begin);

    // hpx::parallel::copy(
    //     hpx::parallel::execution::par, src_begin, src_end, dst_begin);

    std::array<typename Values::index, rank> empty_shape;
    empty_shape.fill(0);
    other._values.resize(empty_shape);

    return *this;
}


template<
    typename Value,
    std::size_t rank>
bool ArrayPartitionData<Value, rank>::operator==(
    ArrayPartitionData const& other) const
{
    return _definition == other._definition && _values == other._values;
}


template<
    typename Value,
    std::size_t rank>
typename ArrayPartitionData<Value, rank>::Definition const&
    ArrayPartitionData<Value, rank>::definition() const
{
    return _definition;
}


template<
    typename Value,
    std::size_t rank>
typename ArrayPartitionData<Value, rank>::size_type
    ArrayPartitionData<Value, rank>::size() const
{
    return _definition.nr_elements();
}


template<
    typename Value,
    std::size_t rank>
typename ArrayPartitionData<Value, rank>::Values const&
    ArrayPartitionData<Value, rank>::values() const
{
    return _values;
}


template<
    typename Value,
    std::size_t rank>
typename ArrayPartitionData<Value, rank>::Values&
    ArrayPartitionData<Value, rank>::values()
{
    return _values;
}


template<
    typename Value,
    std::size_t rank>
Value* ArrayPartitionData<Value, rank>::data()
{
    return _values.data();
}


template<
    typename Value,
    std::size_t rank>
Value const* ArrayPartitionData<Value, rank>::data() const
{
    return _values.data();
}


// template<
//     typename Value,
//     std::size_t rank>
// Value& ArrayPartitionData<Value, rank>::operator[](
//     index const idx)
// {
//     return *(_values.data() + idx);
// }
// 
// 
// template<
//     typename Value,
//     std::size_t rank>
// Value const& ArrayPartitionData<Value, rank>::operator[](
//     index const idx) const
// {
//     return *(_values.data() + idx);
// }

}  // namespace lue
