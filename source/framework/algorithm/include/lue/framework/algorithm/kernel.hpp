#pragma once
#include "lue/framework/core/array.hpp"


namespace lue {

template<
    typename Weight,
    Rank rank>
class Kernel:
    public Array<Weight, rank>
{

    static_assert(rank > 0);

private:

    using Base = Array<Weight, rank>;

public:

    // using Index = typename Base::Index;

    using Shape = typename Base::Shape;

    // using Radius = std::ptrdiff_t;  // std::size_t;

                   Kernel              ()=default;

                   Kernel              (Shape const& shape);

                   Kernel              (Shape const& shape,
                                        Weight weight);

                   Kernel              (Shape const& shape,
                                        std::initializer_list<Weight> weights);

                   Kernel              (Array<Weight, rank> const& weights);

                   Kernel              (Kernel const&)=default;

                   Kernel              (Kernel&&)=default;

                   ~Kernel             ()=default;

    Kernel&        operator=           (Kernel const&)=default;

    Kernel&        operator=           (Kernel&&)=default;

    Radius         radius              () const;

    Count          size                () const;

private:

    Radius         _radius;

};


template<
    typename Weight,
    Rank rank>
Kernel<Weight, rank>::Kernel(
    Shape const& shape):

    Base{shape},
    _radius{(shape[0] - 1) / 2}

{
    assert(nr_elements(shape) > 0);
    assert(this->shape()[0] == this->size());
    assert(is_hypercube(shape));
}


template<
    typename Weight,
    Rank rank>
Kernel<Weight, rank>::Kernel(
    Shape const& shape,
    Weight const weight):

    Base{shape, weight},
    _radius{(shape[0] - 1) / 2}

{
    assert(nr_elements(shape) > 0);
    assert(this->shape()[0] == this->size());
    assert(is_hypercube(shape));
}


template<
    typename Weight,
    Rank rank>
Kernel<Weight, rank>::Kernel(
    Shape const& shape,
    std::initializer_list<Weight> weights):

    Base{shape, weights},
    _radius{(shape[0] - 1) / 2}

{
    assert(nr_elements(shape) > 0);
    assert(this->shape()[0] == this->size());
    assert(is_hypercube(shape));
}


template<
    typename Weight,
    Rank rank>
Kernel<Weight, rank>::Kernel(
    Array<Weight, rank> const& weights):

    Base{weights},
    _radius{(this->shape()[0] - 1) / 2}

{
    assert(nr_elements(this->shape()) > 0);
    assert(this->shape()[0] == this->size());
    assert(is_hypercube(this->shape()));
}


template<
    typename Weight,
    Rank rank>
Radius Kernel<Weight, rank>::radius() const
{
    return _radius;
}


template<
    typename Weight,
    Rank rank>
Size Kernel<Weight, rank>::size() const
{
    return 2 * _radius + 1;
}


template<
    typename Weight,
    Rank rank>
Kernel<Weight, rank> box_kernel(
    Radius const radius,
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
    Rank r>
class ArrayTraits<Kernel<E, r>>
{

public:

    using Element = E;

    constexpr static Rank rank = r;

    using Shape = typename Array<E, r>::Shape;

};

}  // namespace detail
}  // namespace lue
