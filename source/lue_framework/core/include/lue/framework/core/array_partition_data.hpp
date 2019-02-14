#pragma once
#include "lue/framework/core/shape.hpp"
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

    using ValueType = typename Values::element;
    static_assert(std::is_same_v<ValueType, Value>);

    static_assert(std::is_unsigned_v<typename Values::size_type>);
    using SizeType = typename Values::size_type;
    static_assert(std::is_unsigned_v<SizeType>);

    using Index = SizeType;  // typename Values::index;
    static_assert(std::is_unsigned_v<Index>);

    using Shape = lue::Shape<Index, rank>;

    using Iterator = typename Values::iterator;

    using ConstIterator = typename Values::const_iterator;

                   ArrayPartitionData  ();

    explicit       ArrayPartitionData  (Shape const& shape);

                   ArrayPartitionData  (Shape const& shape,
                                        Value const& value);

                   ArrayPartitionData  (ArrayPartitionData const& other);

                   ArrayPartitionData  (ArrayPartitionData&& other);

                   ~ArrayPartitionData ()=default;

    ArrayPartitionData& operator=      (ArrayPartitionData const& other);

    ArrayPartitionData& operator=      (ArrayPartitionData&& other);

    bool           operator==          (ArrayPartitionData const& other) const;

    Shape const&   shape               () const;

    SizeType       size                () const;

    Values const&  values              () const;

    Values&        values              ();

    Value*         data                ();

    Value const*   data                () const;

    ConstIterator  begin               () const;

    Iterator       begin               ();

    ConstIterator  end                 () const;

    Iterator       end                 ();

    // Value&         operator[]          (Index idx);

    // Value const&   operator[]          (Index idx) const;

private:

    Shape          _shape;

    Values         _values;

};


template<
    typename Value,
    std::size_t rank>
ArrayPartitionData<Value, rank>::ArrayPartitionData():

    _shape{},
    _values{}

{
}


template<
    typename Value,
    std::size_t rank>
ArrayPartitionData<Value, rank>::ArrayPartitionData(
    Shape const& shape):

    _shape{shape},
    _values{shape}

{
}


template<
    typename Value,
    std::size_t rank>
ArrayPartitionData<Value, rank>::ArrayPartitionData(
    Shape const& shape,
    Value const& value):

    _shape{shape},
    _values{shape}

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

    _shape{other._shape},
    _values{other._shape}

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

    _shape{},
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
    _shape = other._shape;
    _values.resize(other._shape);

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
    _shape = std::move(other._shape);

    // Copy values and empty the source array afterwards
    _values.resize(_shape);

    auto const src_begin = other._values.data();
    auto const src_end = src_begin + other._values.num_elements();
    auto dst_begin = _values.data();

    std::copy(src_begin, src_end, dst_begin);

    // hpx::parallel::copy(
    //     hpx::parallel::execution::par, src_begin, src_end, dst_begin);

    std::array<Index, rank> empty_shape;
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
    return _shape == other._shape && _values == other._values;
}


template<
    typename Value,
    std::size_t rank>
typename ArrayPartitionData<Value, rank>::Shape const&
    ArrayPartitionData<Value, rank>::shape() const
{
    return _shape;
}


template<
    typename Value,
    std::size_t rank>
typename ArrayPartitionData<Value, rank>::SizeType
    ArrayPartitionData<Value, rank>::size() const
{
    return nr_elements(_shape);
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


template<
    typename Value,
    std::size_t rank>
typename ArrayPartitionData<Value, rank>::ConstIterator
    ArrayPartitionData<Value, rank>::begin() const
{
    return _values.begin();
}


template<
    typename Value,
    std::size_t rank>
typename ArrayPartitionData<Value, rank>::Iterator
    ArrayPartitionData<Value, rank>::begin()
{
    return _values.begin();
}


template<
    typename Value,
    std::size_t rank>
typename ArrayPartitionData<Value, rank>::ConstIterator
    ArrayPartitionData<Value, rank>::end() const
{
    return _values.end();
}


template<
    typename Value,
    std::size_t rank>
typename ArrayPartitionData<Value, rank>::Iterator
    ArrayPartitionData<Value, rank>::end()
{
    return _values.end();
}


// template<
//     typename Value,
//     std::size_t rank>
// Value& ArrayPartitionData<Value, rank>::operator[](
//     Index const idx)
// {
//     return *(_values.data() + idx);
// }
// 
// 
// template<
//     typename Value,
//     std::size_t rank>
// Value const& ArrayPartitionData<Value, rank>::operator[](
//     Index const idx) const
// {
//     return *(_values.data() + idx);
// }


template<
    typename Value>
class ArrayPartitionData<Value, 0>
{

public:

    static const auto rank = 0;

    using Values = std::array<Value, 1>;

    using ValueType = typename Values::value_type;
    static_assert(std::is_same_v<ValueType, Value>);

    static_assert(std::is_unsigned_v<typename Values::size_type>);
    using SizeType = typename Values::size_type;
    static_assert(std::is_unsigned_v<SizeType>);

    using Index = SizeType;
    static_assert(std::is_unsigned_v<Index>);

    using Shape = lue::Shape<Index, 0>;

    using Iterator = typename Values::iterator;

    using ConstIterator = typename Values::const_iterator;

                   ArrayPartitionData  ();

    explicit       ArrayPartitionData  (Value const& value);

                   ArrayPartitionData  (Shape const& shape,
                                        Value const& value);

                   ArrayPartitionData  (ArrayPartitionData const&)=default;

                   ArrayPartitionData  (ArrayPartitionData&&)=default;

                   ~ArrayPartitionData ()=default;

    ArrayPartitionData& operator=      (ArrayPartitionData const&)=default;

    ArrayPartitionData& operator=      (ArrayPartitionData&&)=default;

    bool           operator==          (ArrayPartitionData const& other) const;

    Shape const&   shape               () const;

    SizeType       size                () const;

    Values const&  values              () const;

    Values&        values              ();

    ConstIterator  begin               () const;

    Iterator       begin               ();

    ConstIterator  end                 () const;

    Iterator       end                 ();

private:

    Shape          _shape;

    Values         _values;

};


template<
    typename Value>
ArrayPartitionData<Value, 0>::ArrayPartitionData():

    _shape{},
    _values{}

{
    assert(_values.size() == 1);
}


template<
    typename Value>
ArrayPartitionData<Value, 0>::ArrayPartitionData(
    Value const& value):

    _shape{},
    _values{}

{
    _values[0] = value;

    assert(_values.size() == 1);
}


template<
    typename Value>
ArrayPartitionData<Value, 0>::ArrayPartitionData(
    Shape const& /* shape */,
    Value const& value):

    ArrayPartitionData{value}

{
    assert(_values.size() == 1);
}


template<
    typename Value>
bool ArrayPartitionData<Value, 0>::operator==(
    ArrayPartitionData const& other) const
{
    assert(_shape == other._shape);

    return _values[0] == other._values[0];
}


template<
    typename Value>
typename ArrayPartitionData<Value, 0>::Shape const&
    ArrayPartitionData<Value, 0>::shape() const
{
    return _shape;
}


template<
    typename Value>
typename ArrayPartitionData<Value, 0>::SizeType
    ArrayPartitionData<Value, 0>::size() const
{
    return nr_elements(_shape);
}


template<
    typename Value>
typename ArrayPartitionData<Value, 0>::Values const&
    ArrayPartitionData<Value, 0>::values() const
{
    return _values;
}


template<
    typename Value>
typename ArrayPartitionData<Value, 0>::Values&
    ArrayPartitionData<Value, 0>::values()
{
    return _values;
}


template<
    typename Value>
typename ArrayPartitionData<Value, 0>::ConstIterator
    ArrayPartitionData<Value, 0>::begin() const
{
    return _values.begin();
}


template<
    typename Value>
typename ArrayPartitionData<Value, 0>::Iterator
    ArrayPartitionData<Value, 0>::begin()
{
    return _values.begin();
}


template<
    typename Value>
typename ArrayPartitionData<Value, 0>::ConstIterator
    ArrayPartitionData<Value, 0>::end() const
{
    return _values.end();
}


template<
    typename Value>
typename ArrayPartitionData<Value, 0>::Iterator
    ArrayPartitionData<Value, 0>::end()
{
    return _values.end();
}


}  // namespace lue
