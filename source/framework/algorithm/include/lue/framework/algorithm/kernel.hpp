#pragma once
#include "lue/framework/core/array.hpp"


namespace lue {

template<
    typename Weight,
    std::size_t rank>
class Kernel:
    public Array<Weight, rank>
{

    static_assert(rank > 0);

private:

    using Base = Array<Weight, rank>;

public:

    using Shape = typename Base::Shape;

                   Kernel              (Shape const& shape);

                   Kernel              (Shape const& shape,
                                        Weight weight);

                   Kernel              (Kernel const&)=default;

                   Kernel              (Kernel&&)=default;

                   ~Kernel             ()=default;

    Kernel&        operator=           (Kernel const&)=default;

    Kernel&        operator=           (Kernel&&)=default;

    std::size_t    radius              () const;

private:

    std::size_t const _radius;

};


template<
    typename Weight,
    std::size_t rank>
Kernel<Weight, rank>::Kernel(
    Shape const& shape):

    Base{shape},
    _radius{(shape[0] - 1) / 2}

{
    assert(nr_elements(shape) > 0);
    assert(2 * _radius + 1 == shape[0]);
    assert(is_hypercube(shape));
}


template<
    typename Weight,
    std::size_t rank>
Kernel<Weight, rank>::Kernel(
    Shape const& shape,
    Weight const weight):

    Base{shape, weight},
    _radius{(shape[0] - 1) / 2}

{
    assert(nr_elements(shape) > 0);
    assert(2 * _radius + 1 == shape[0]);
    assert(is_hypercube(shape));
}


template<
    typename Weight,
    std::size_t rank>
std::size_t Kernel<Weight, rank>::radius() const
{
    return _radius;
}


template<
    typename Weight,
    std::size_t rank>
Kernel<Weight, rank> square_kernel(
    std::size_t const radius,
    Weight const value)
{
    using Kernel = Kernel<Weight, rank>;
    using Shape = ShapeT<Kernel>;

    Shape shape{};
    std::fill(shape.begin(), shape.end(), 2 * radius + 1);
    Kernel kernel{shape, value};

    return kernel;
}


namespace detail {

template<
    typename E,
    std::size_t r>
class ArrayTraits<Kernel<E, r>>
{

public:

    using Element = E;

    constexpr static std::size_t rank = r;

    using Shape = typename Array<E, r>::Shape;

};

}  // namespace detail
}  // namespace lue
