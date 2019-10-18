#pragma once
#include "lue/framework/core/type_traits.hpp"
#include "lue/framework/core/shape.hpp"
#include <boost/container/vector.hpp>
#include <cassert>


namespace lue {
namespace detail {

template<
    typename Element,
    std::size_t rank>
class Array
{

public:

    using Index = std::size_t;

    using Shape = lue::Shape<Index, rank>;

    using Elements = boost::container::vector<Element>;

    using Iterator = typename Elements::iterator;

    using ConstIterator = typename Elements::const_iterator;

                   Array               ()=default;

                   Array               (Shape const& shape);

                   Array               (Shape const& shape,
                                        Element value);

                   Array               (Array const&)=default;

                   Array               (Array&&)=default;

                   ~Array              ()=default;

    Array&         operator=           (Array const&)=default;

    Array&         operator=           (Array&&)=default;

    Shape const& shape() const
    {
        return _shape;
    }

    Iterator       begin               ();

    Iterator       end                 ();

    ConstIterator  begin               () const;

    ConstIterator  end                 () const;

protected:

    Element const* data                () const;

    Element*       data                ();

private:

    Shape          _shape;

    Elements       _elements;

};


template<
    typename Element,
    std::size_t rank>
Array<Element, rank>::Array(
    Shape const& shape):

    _shape{shape},
    _elements(lue::nr_elements(shape))

{
}


template<
    typename Element,
    std::size_t rank>
Array<Element, rank>::Array(
    Shape const& shape,
    Element const value):

    _shape{shape},
    _elements(lue::nr_elements(shape), value)

{
}


template<
    typename Element,
    std::size_t rank>
typename Array<Element, rank>::Iterator Array<Element, rank>::begin()
{
    return _elements.begin();
}


template<
    typename Element,
    std::size_t rank>
typename Array<Element, rank>::ConstIterator Array<Element, rank>::begin() const
{
    return _elements.begin();
}


template<
    typename Element,
    std::size_t rank>
typename Array<Element, rank>::Iterator Array<Element, rank>::end()
{
    return _elements.end();
}


template<
    typename Element,
    std::size_t rank>
typename Array<Element, rank>::ConstIterator Array<Element, rank>::end() const
{
    return _elements.end();
}


template<
    typename Element,
    std::size_t rank>
Element const* Array<Element, rank>::data() const
{
    return _elements.data();
}


template<
    typename Element,
    std::size_t rank>
Element* Array<Element, rank>::data()
{
    return _elements.data();
}

} // namespace detail


template<
    typename Element,
    std::size_t rank>
class Array:
    public detail::Array<Element, rank>
{

public:

    using detail::Array<Element, rank>::Array;

    Element const* operator[]          (std::size_t idx) const;

    Element*       operator[]          (std::size_t idx);

};


template<
    typename Element,
    std::size_t rank>
Element const* Array<Element, rank>::operator[](
    std::size_t const idx) const
{
    return this->data() + idx;
}


template<
    typename Element,
    std::size_t rank>
Element* Array<Element, rank>::operator[](
    std::size_t const idx)
{
    return this->data() + idx;
}


template<
    typename Element>
class Array<Element, 1>:
    public detail::Array<Element, 1>
{

public:

    using detail::Array<Element, 1>::Array;

    Element&       operator[]          (std::size_t idx);

    Element const& operator[]          (std::size_t idx) const;

};


template<
    typename Element>
Element const& Array<Element, 1>::operator[](
    std::size_t const idx) const
{
    return this->data()[idx];
}


template<
    typename Element>
Element& Array<Element, 1>::operator[](
    std::size_t const idx)
{
    return this->data()[idx];
}


namespace detail {

template<
    typename E,
    std::size_t r>
class ArrayTraits<lue::Array<E, r>>
{

public:

    using Element = E;

    constexpr static std::size_t rank = r;

    using Shape = typename lue::Array<E, r>::Shape;

};

}  // namespace detail


template<
    typename Element,
    std::size_t rank>
inline auto const& shape(
    Array<Element, rank> const& array)
{
    return array.shape();
}


template<
    typename Element,
    std::size_t rank>
inline auto nr_elements(
    Array<Element, rank> const& array)
{
    return nr_elements(shape(array));
}

}  // namespace lue
