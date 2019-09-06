#pragma once
#include "lue/framework/core/shape.hpp"
#include "lue/framework/core/serialize/shared_buffer.hpp"
#include "lue/framework/core/type_traits.hpp"
#include <hpx/runtime/serialization/serialize.hpp>
// #ifndef NDEBUG
//     #define BOOST_DISABLE_ASSERTS
// #endif
// #define BOOST_MULTI_ARRAY_NO_GENERATORS
// #include <boost/multi_array.hpp>
// #include <boost/container/vector.hpp>
#include <algorithm>
#include <memory>


namespace lue {

/*!
    @brief      Class for keeping track of array partition data values
    @warning    Copies of ArrayPartitionData instances share the
                underlying array with elements

    There is a specialization for array scalars (arrays with rank ==
    0). This allows scalars to be handled the same as arrays.
*/
template<
    typename Value,
    std::size_t rank_>
class ArrayPartitionData
{

private:

    // using Values = boost::multi_array<Value, rank>;
    // using Values = std::vector<Value>;
    // using Values = boost::container::vector<Value>;
    using Values = SharedBuffer<Value>;

public:

    static const auto rank = rank_;

    // using ValueType = typename Values::element;
    using ValueType = typename Values::value_type;
    static_assert(std::is_same_v<ValueType, Value>);

    using ElementType = ValueType;
    using Element = ElementType;

    static_assert(std::is_unsigned_v<typename Values::size_type>);
    using SizeType = typename Values::size_type;
    static_assert(std::is_unsigned_v<SizeType>);

    using Size = SizeType;

    using Index = SizeType;  // typename Values::index;
    static_assert(std::is_unsigned_v<Index>);

    using Shape = lue::Shape<Index, rank>;
    using ShapeType = Shape;

    using Iterator = typename Values::iterator;

    using ConstIterator = typename Values::const_iterator;

    // using Iterator = ElementType*;

    // using ConstIterator = ElementType const*;

                   ArrayPartitionData  ();

    explicit       ArrayPartitionData  (Shape const& shape);

                   ArrayPartitionData  (Shape const& shape,
                                        Value const& value);

                   ArrayPartitionData  (ArrayPartitionData const& other,
                                        CopyMode mode=CopyMode::copy);

                   ArrayPartitionData  (ArrayPartitionData&& other);

                   ~ArrayPartitionData ()=default;

    ArrayPartitionData& operator=      (ArrayPartitionData const& other);

    ArrayPartitionData& operator=      (ArrayPartitionData&& other);

    bool           operator==          (ArrayPartitionData const& other) const;

    Shape const&   shape               () const;

    SizeType       size                () const;

    bool           empty               () const;

    void           resize              (Shape const& shape);

    // Value*         data                ();

    // Value const*   data                () const;

    ConstIterator  begin               () const;

    Iterator       begin               ();

    ConstIterator  end                 () const;

    Iterator       end                 ();

    Value&         operator[]          (Index idx);

    Value const&   operator[]          (Index idx) const;

private:

    Values const&  values              () const;

    Values&        values              ();

    // All arguments passed to actions must support serialization,
    // even if they are never actually communicated between localities
    friend class hpx::serialization::access;

    template<typename Archive>
    void serialize(
        Archive& archive,
        unsigned int const /* version */)
    {
        archive & _shape & _values;
    }

    Shape          _shape;

    // std::shared_ptr<Values> _values;
    Values         _values;

};


/*!
    @brief      Default-construct an instance

    The instance will be an empty partition.
*/
template<
    typename Value,
    std::size_t rank>
ArrayPartitionData<Value, rank>::ArrayPartitionData():

    _shape{},
    // _values{std::make_shared<Values>()}
    _values{}

{
    assert(_values.size() == nr_elements(_shape));
}


/*!
    @brief      Construct an instance with shape @a shape
    @param      shape Shape of data array
*/
template<
    typename Value,
    std::size_t rank>
ArrayPartitionData<Value, rank>::ArrayPartitionData(
    Shape const& shape):

    _shape{shape},
    // _values{std::make_shared<Values>(shape)}
    // _values{shape}
    _values{nr_elements(shape)}

{
    assert(_values.size() == nr_elements(_shape));
}


/*!
    @brief      Construct an instance with a certain shape and fill-value
    @param      shape Shape of data array to create
    @param      value Value to fill array with
*/
template<
    typename Value,
    std::size_t rank>
ArrayPartitionData<Value, rank>::ArrayPartitionData(
    Shape const& shape,
    Value const& value):

    // ArrayPartitionData{shape}

    _shape{shape},
    // _values{std::make_shared<Values>(shape)}
    // _values{shape}
    _values(nr_elements(shape))

{
    // std::fill_n(_values->data(), _values->num_elements(), value);
    // std::fill_n(_values.data(), _values.num_elements(), value);
    std::fill_n(_values.begin(), _values.size(), value);

    // // hpx::parallel::fill_n(
    // //     hpx::parallel::execution::par,
    // //     _values.data(), _values.num_elements(), value);

    assert(_values.size() == nr_elements(_shape));
}


/*!
    @brief      Copy-construct an instance based on @a other
*/
template<
    typename Value,
    std::size_t rank>
ArrayPartitionData<Value, rank>::ArrayPartitionData(
    ArrayPartitionData const& other,
    CopyMode const mode):

    _shape{other._shape},
    _values{other._values, mode}

    // _values{Values{
    //     other._values.data(), other._values.size(), Values::Mode::copy}}

{
    // auto const src_begin = other._values.data();
    // auto const src_end = src_begin + other._values.num_elements();
    // auto dst_begin = _values.data();

    // std::copy(src_begin, src_end, dst_begin);

    // // hpx::parallel::copy(
    // //     hpx::parallel::execution::par, src_begin, src_end, dst_begin);

    assert(
        (mode == CopyMode::copy && (_values.data() != other._values.data())) ||
        (mode == CopyMode::share && (_values.data() == other._values.data()))
    );
    assert(_values.size() == nr_elements(_shape));
}


/*!
    @brief      Move-construct and instance based on @a other
*/
template<
    typename Value,
    std::size_t rank>
ArrayPartitionData<Value, rank>::ArrayPartitionData(
    ArrayPartitionData&& other):

    _shape{std::move(other._shape)},
    _values{std::move(other._values)}

{
    assert(_values.data() != other._values.data());
    assert(_values.size() == nr_elements(_shape));
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
    _values = Values{
        other._values.data(), other._values.size(), Values::Mode::copy};

    // _values.resize(other._shape);

    // auto const src_begin = other._values.data();
    // auto const src_end = src_begin + other._values.num_elements();
    // auto dst_begin = _values.data();

    // std::copy(src_begin, src_end, dst_begin);

    // // hpx::parallel::copy(
    // //     hpx::parallel::execution::par, src_begin, src_end, dst_begin);

    assert(_values.data() != other._values.data());
    assert(_values.size() == nr_elements(_shape));

    return *this;
}


/*!
    @brief      Move assign @a other to this instance
*/
template<
    typename Value,
    std::size_t rank>
ArrayPartitionData<Value, rank>& ArrayPartitionData<Value, rank>::operator=(
    ArrayPartitionData&& other)
{
    _shape = std::move(other._shape);
    _values = std::move(other._values);

    // // Copy values and empty the source array afterwards
    // _values.resize(_shape);

    // auto const src_begin = other._values.data();
    // auto const src_end = src_begin + other._values.num_elements();
    // auto dst_begin = _values.data();

    // std::copy(src_begin, src_end, dst_begin);

    // // hpx::parallel::copy(
    // //     hpx::parallel::execution::par, src_begin, src_end, dst_begin);

    // std::array<Index, rank> empty_shape;
    // empty_shape.fill(0);
    // other._values.resize(empty_shape);

    assert(_values.data() != other._values.data());
    assert(_values.size() == nr_elements(_shape));

    return *this;
}


template<
    typename Value,
    std::size_t rank>
bool ArrayPartitionData<Value, rank>::operator==(
    ArrayPartitionData const& other) const
{
    return _shape == other._shape && values() == other.values();
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
    // assert(nr_elements(_shape) == values().num_elements());
    assert(nr_elements(_shape) == values().size());

    return nr_elements(_shape);
}


template<
    typename Value,
    std::size_t rank>
bool ArrayPartitionData<Value, rank>::empty() const
{
    return size() == 0;
}


template<
    typename Value,
    std::size_t rank>
void ArrayPartitionData<Value, rank>::resize(
    Shape const& shape)
{
    // values().resize(shape);
    values().resize(nr_elements(shape));
    _shape = shape;
}


template<
    typename Value,
    std::size_t rank>
typename ArrayPartitionData<Value, rank>::Values const&
    ArrayPartitionData<Value, rank>::values() const
{
    // return *_values;
    return _values;
}


template<
    typename Value,
    std::size_t rank>
typename ArrayPartitionData<Value, rank>::Values&
    ArrayPartitionData<Value, rank>::values()
{
    // return *_values;
    return _values;
}


// template<
//     typename Value,
//     std::size_t rank>
// Value* ArrayPartitionData<Value, rank>::data()
// {
//     return values().data();
// }
// 
// 
// template<
//     typename Value,
//     std::size_t rank>
// Value const* ArrayPartitionData<Value, rank>::data() const
// {
//     return values().data();
// }


template<
    typename Value,
    std::size_t rank>
typename ArrayPartitionData<Value, rank>::ConstIterator
    ArrayPartitionData<Value, rank>::begin() const
{
    // return values().data();
    return values().begin();
}


template<
    typename Value,
    std::size_t rank>
typename ArrayPartitionData<Value, rank>::Iterator
    ArrayPartitionData<Value, rank>::begin()
{
    // return values().data();
    return values().begin();
}


template<
    typename Value,
    std::size_t rank>
typename ArrayPartitionData<Value, rank>::ConstIterator
    ArrayPartitionData<Value, rank>::end() const
{
    // return begin() + values().num_elements();
    return values().end();
}


template<
    typename Value,
    std::size_t rank>
typename ArrayPartitionData<Value, rank>::Iterator
    ArrayPartitionData<Value, rank>::end()
{
    // return begin() + values().num_elements();
    return values().end();
}


template<
    typename Value,
    std::size_t rank>
Value& ArrayPartitionData<Value, rank>::operator[](
    Index const idx)
{
    // assert(idx < size());

    // return *(this->begin() + idx);

    return _values[idx];
}


template<
    typename Value,
    std::size_t rank>
Value const& ArrayPartitionData<Value, rank>::operator[](
    Index const idx) const
{
    // assert(idx < size());

    // return *(this->begin() + idx);

    return _values[idx];
}


template<
    typename Value>
class ArrayPartitionData<Value, 0>
{

private:

    using Values = std::array<Value, 1>;

public:

    static const auto rank = 0;

    using ValueType = typename Values::value_type;
    static_assert(std::is_same_v<ValueType, Value>);

    static_assert(std::is_unsigned_v<typename Values::size_type>);
    using SizeType = typename Values::size_type;
    static_assert(std::is_unsigned_v<SizeType>);

    using Size = SizeType;

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

    bool           empty               () const;

    void           resize              (Shape const& shape);

    // Values const&  values              () const;

    // Values&        values              ();

    // Value*         data                ();

    // Value const*   data                () const;

    ConstIterator  begin               () const;

    Iterator       begin               ();

    ConstIterator  end                 () const;

    Iterator       end                 ();

    Value&         operator[]          (Index idx);

    Value const&   operator[]          (Index idx) const;

private:

    // All arguments passed to actions must support serialization,
    // even if they are never actually communicated between localities
    friend class hpx::serialization::access;

    template<typename Archive>
    void serialize(Archive& /* archive */, unsigned int const /* version */)
    {
        // For now, we do nothing. Revisit later.
        HPX_ASSERT(false);
    }

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
void ArrayPartitionData<Value, 0>::resize(
    Shape const& /* shape */)
{
    // Cannot resize a scalar array
    HPX_ASSERT(false);
}


template<
    typename Value>
typename ArrayPartitionData<Value, 0>::SizeType
    ArrayPartitionData<Value, 0>::size() const
{
    assert(nr_elements(_shape) == 0);
    assert(_values.size() == 1);

    return 1;
}


template<
    typename Value>
bool ArrayPartitionData<Value, 0>::empty() const
{
    return false;
}


// template<
//     typename Value>
// typename ArrayPartitionData<Value, 0>::Values const&
//     ArrayPartitionData<Value, 0>::values() const
// {
//     return _values;
// }
// 
// 
// template<
//     typename Value>
// typename ArrayPartitionData<Value, 0>::Values&
//     ArrayPartitionData<Value, 0>::values()
// {
//     return _values;
// }
// 
// 
// template<
//     typename Value>
// Value* ArrayPartitionData<Value, 0>::data()
// {
//     return _values.begin();
// }
// 
// 
// template<
//     typename Value>
// Value const* ArrayPartitionData<Value, 0>::data() const
// {
//     return _values.begin();
// }


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


template<
    typename Value>
Value& ArrayPartitionData<Value, 0>::operator[](
    [[maybe_unused]] Index idx)
{
    assert(idx == 0);

    return _values[0];
}


template<
    typename Value>
Value const& ArrayPartitionData<Value, 0>::operator[](
    [[maybe_unused]] Index idx) const
{
    assert(idx == 0);

    return _values[0];
}


namespace detail {

template<
    typename E,
    std::size_t r>
class ArrayTraits<ArrayPartitionData<E, r>>
{

public:

    using Element = E;

    constexpr static std::size_t rank = r;

};

}  // namespace detail
}  // namespace lue
