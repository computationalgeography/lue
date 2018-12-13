#pragma once
#include "lue/framework/core/shape.hpp"


namespace lue {

template<
    typename T,
    std::size_t rank>
class Grain
{

public:

    using Start = lue::Shape<T, rank>;
    using Shape = lue::Shape<T, rank>;

                   Grain               ()=default;

                   Grain               (Shape const& shape);

                   Grain               (Start const& start,
                                        Shape const& shape);

                   Grain               (Grain const&)=default;

                   Grain               (Grain&&)=default;

                   ~Grain              ()=default;

    Grain&         operator=           (Grain const&)=default;

    Grain&         operator=           (Grain&&)=default;

    bool           operator==          (Grain const& other);

    Start const&   start               () const;

    Start&         start               ();

    Shape const&   shape               () const;

    Shape&         shape               ();

private:

    Start          _start;

    Shape          _shape;

};


template<
    typename T,
    std::size_t rank>
inline Grain<T, rank>::Grain(
    Shape const& shape):

    _start{},
    _shape{shape}

{
}


template<
    typename T,
    std::size_t rank>
inline Grain<T, rank>::Grain(
    Start const& start,
    Shape const& shape):

    _start{start},
    _shape{shape}

{
}


template<
    typename T,
    std::size_t rank>
inline bool Grain<T, rank>::operator==(
    Grain const& other)
{
    return _start == other._start && _shape == other._shape;
}


template<
    typename T,
    std::size_t rank>
inline typename Grain<T, rank>::Start const& Grain<T, rank>::start() const
{
    return _start;
}


template<
    typename T,
    std::size_t rank>
inline typename Grain<T, rank>::Start& Grain<T, rank>::start()
{
    return _start;
}


template<
    typename T,
    std::size_t rank>
inline typename Grain<T, rank>::Shape const& Grain<T, rank>::shape() const
{
    return _shape;
}


template<
    typename T,
    std::size_t rank>
inline typename Grain<T, rank>::Shape& Grain<T, rank>::shape()
{
    return _shape;
}

}  // namespace lue
