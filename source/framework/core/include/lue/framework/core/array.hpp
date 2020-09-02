#pragma once
#include "lue/framework/core/debug.hpp"
#include "lue/framework/core/define.hpp"
#include "lue/framework/core/erase.hpp"
#include "lue/framework/core/shape.hpp"
#include "lue/framework/core/span.hpp"
#include "lue/framework/core/type_traits.hpp"
#include "lue/assert.hpp"
#include "lue/configure.hpp"
#include <boost/container/vector.hpp>


namespace lue {
// namespace detail {

template<
    typename Element,
    Rank rank>
class Array
{

public:

    using Span = DynamicSpan<Element, rank>;

    using Shape = lue::Shape<lue::Index, rank>;

    using Elements = boost::container::vector<Element>;

    using Iterator = typename Elements::iterator;

    using ConstIterator = typename Elements::const_iterator;

                   Array               ()=default;

    explicit       Array               (Shape const& shape);

                   Array               (Shape const& shape,
                                        Element value);

                   Array               (Shape const& shape,
                                        std::initializer_list<Element> values);

    template<
        typename InputIterator>
                   Array               (Shape const& shape,
                                        InputIterator begin,
                                        InputIterator end);

                   Array               (Array const& other);

                   Array               (Array&& other);

                   ~Array              ()=default;

    Array&         operator=           (Array const& other);

    Array&         operator=           (Array&& other);

    bool empty() const
    {
        return _elements.empty();
    }

    Shape const&   shape               () const;

    Iterator       begin               ();

    Iterator       end                 ();

    ConstIterator  begin               () const;

    ConstIterator  end                 () const;

    void           reshape             (Shape const& shape);

    void           erase               (Rank const dimension_idx,
                                        Index const hyperslab_begin_idx,
                                        Index const hyperslab_end_idx);

    Element const* data                () const;

    Element*       data                ();

    template<
        typename... Idxs>
    Element const& operator()(
        Idxs... idxs) const
    {
        if constexpr(BuildOptions::validate_idxs)
        {
            validate_idxs(_shape, idxs...);
        }

        return _span(idxs...);
    }

    template<
        typename... Idxs>
    Element& operator()(
        Idxs... idxs)
    {
        if constexpr(BuildOptions::validate_idxs)
        {
            validate_idxs(_shape, idxs...);
        }

        return _span(idxs...);
    }

    Element const  operator[]          (std::size_t idx) const;

    Element        operator[]          (std::size_t idx);

protected:

private:

    void           assert_invariants   () const;

    // Shape of array
    Shape          _shape;

    // 1D buffer with array elements
    Elements       _elements;

    // Span for converting nD indices to linear indices
    Span           _span;

};


template<
    typename Element,
    Rank rank>
Array<Element, rank>::Array(
    Shape const& shape):

    _shape{shape},
    _elements(lue::nr_elements(shape)),
    _span{_elements.data(), _shape}

{
    assert_invariants();
}


template<
    typename Element,
    Rank rank>
Array<Element, rank>::Array(
    Shape const& shape,
    Element const value):

    _shape{shape},
    _elements(lue::nr_elements(shape), value),
    _span{_elements.data(), _shape}

{
    assert_invariants();
}


template<
    typename Element,
    Rank rank>
template<
    typename InputIterator>
Array<Element, rank>::Array(
    Shape const& shape,
    InputIterator begin,
    InputIterator end):

    _shape{shape},
    _elements(lue::nr_elements(shape)),
    _span{_elements.data(), _shape}

{
    std::move(begin, end, _elements.begin());

    assert_invariants();
}


template<
    typename Element,
    Rank rank>
Array<Element, rank>::Array(
    Shape const& shape,
    std::initializer_list<Element> values):

    _shape{shape},
    _elements(lue::nr_elements(shape)),
    _span{_elements.data(), _shape}

{
    lue_assert(
        std::distance(values.begin(), values.end()) ==
        lue::nr_elements(_shape));

    std::move(values.begin(), values.end(), _elements.begin());

    assert_invariants();
}


template<
    typename Element,
    Rank rank>
Array<Element, rank>::Array(
    Array const& other):

    _shape{other._shape},
    _elements{other._elements},
    _span{_elements.data(), _shape}

{
    assert_invariants();
}


template<
    typename Element,
    Rank rank>
Array<Element, rank>::Array(
    Array&& other):

    _shape{std::move(other._shape)},
    _elements{std::move(other._elements)},
    _span{_elements.data(), _shape}

{
    other._shape.fill(0);
    other._elements.clear();
    other._span = Span{other._elements.data(), other._shape};

    other.assert_invariants();
    lue_assert(other.empty());

    assert_invariants();
}


template<
    typename Element,
    Rank rank>
Array<Element, rank>& Array<Element, rank>::operator=(
    Array const& other)
{
    if(this != &other)
    {
        _shape = other._shape;
        _elements = other._elements;
        _span = Span{_elements.data(), _shape};
    }

    assert_invariants();

    return *this;
}


template<
    typename Element,
    Rank rank>
Array<Element, rank>& Array<Element, rank>::operator=(
    Array&& other)
{
    if(this != &other)
    {
        _shape = std::move(other._shape);
        other._shape.fill(0);

        _elements = std::move(other._elements);
        other._elements.clear();

        _span = Span{_elements.data(), _shape};
        other._span = Span{other._elements.data(), other._shape};
    }

    other.assert_invariants();
    lue_assert(other.empty());
    assert_invariants();

    return *this;
}


template<
    typename Element,
    Rank rank>
typename Array<Element, rank>::Shape const& Array<Element, rank>::shape() const
{
    return _shape;
}


template<
    typename Element,
    Rank rank>
typename Array<Element, rank>::Iterator Array<Element, rank>::begin()
{
    return _elements.begin();
}


template<
    typename Element,
    Rank rank>
typename Array<Element, rank>::ConstIterator Array<Element, rank>::begin() const
{
    return _elements.begin();
}


template<
    typename Element,
    Rank rank>
typename Array<Element, rank>::Iterator Array<Element, rank>::end()
{
    return _elements.end();
}


template<
    typename Element,
    Rank rank>
typename Array<Element, rank>::ConstIterator Array<Element, rank>::end() const
{
    return _elements.end();
}


template<
    typename Element,
    Rank rank>
Element const* Array<Element, rank>::data() const
{
    return _elements.data();
}


template<
    typename Element,
    Rank rank>
Element* Array<Element, rank>::data()
{
    return _elements.data();
}


template<
    typename Element,
    Rank rank>
void Array<Element, rank>::reshape(
    Shape const& shape)
{
    if(_shape != shape) {
        _elements.resize(lue::nr_elements(shape));
        _shape = shape;
        _span = Span{_elements.data(), _shape};
    }

    assert_invariants();
}


template<
    typename Element,
    Rank rank>
void Array<Element, rank>::erase(
    Rank const dimension_idx,
    Index const hyperslab_begin_idx,
    Index const hyperslab_end_idx)
{
    _shape = lue::erase(_elements, _shape, dimension_idx, hyperslab_begin_idx, hyperslab_end_idx);
    _span = Span{_elements.data(), _shape};

    assert_invariants();
}

// } // namespace detail


// template<
//     typename Element,
//     Rank rank>
// class Array:
//     public detail::Array<Element, rank>
// {
// 
// public:
// 
//     using detail::Array<Element, rank>::Array;
// 
//     Element const  operator[]          (std::size_t idx) const;
// 
//     Element        operator[]          (std::size_t idx);
// 
// };


template<
    typename Element,
    Rank rank>
Element const Array<Element, rank>::operator[](
    std::size_t const idx) const
{
    return this->data()[idx];
}


template<
    typename Element,
    Rank rank>
Element Array<Element, rank>::operator[](
    std::size_t const idx)
{
    return this->data()[idx];
}


template<
    typename Element,
    Rank rank>
void Array<Element, rank>::assert_invariants() const
{
    // lue_assert(
    //     static_cast<Size>(_elements.size()) == lue::nr_elements(_shape));
    lue_assert(lue::nr_elements(_shape) == static_cast<Size>(_elements.size()));
    lue_assert(_span.size() == static_cast<Size>(_elements.size()));
    lue_assert(_span.data() == _elements.data());
}


// template<
//     typename Element>
// class Array<Element, 1>:
//     public detail::Array<Element, 1>
// {
// 
// public:
// 
//     using detail::Array<Element, 1>::Array;
// 
//     Element&       operator[]          (Index idx);
// 
//     Element const& operator[]          (Index idx) const;
// 
// };
// 
// 
// template<
//     typename Element>
// Element const& Array<Element, 1>::operator[](
//     Index const idx) const
// {
//     return this->data()[idx];
// }
// 
// 
// template<
//     typename Element>
// Element& Array<Element, 1>::operator[](
//     Index const idx)
// {
//     return this->data()[idx];
// }


namespace detail {

template<
    typename E,
    Rank r>
class ArrayTraits<lue::Array<E, r>>
{

public:

    using Element = E;

    constexpr static Rank rank = r;

    using Shape = typename lue::Array<E, r>::Shape;

};

}  // namespace detail


template<
    typename Element,
    Rank rank>
inline auto const& shape(
    Array<Element, rank> const& array)
{
    return array.shape();
}


template<
    typename Element,
    Rank rank>
inline auto nr_elements(
    Array<Element, rank> const& array)
{
    return nr_elements(shape(array));
}

}  // namespace lue
