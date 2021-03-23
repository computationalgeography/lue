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

    explicit       Kernel              (Shape const& shape);

                   Kernel              (Shape const& shape,
                                        Weight weight);

                   Kernel              (Shape const& shape,
                                        std::initializer_list<Weight> weights);

    explicit       Kernel              (Array<Weight, rank> const& weights);

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
    lue_hpx_assert(nr_elements(shape) > 0);
    lue_hpx_assert(this->shape()[0] == this->size());
    lue_hpx_assert(is_hypercube(shape));
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
    lue_hpx_assert(nr_elements(shape) > 0);
    lue_hpx_assert(this->shape()[0] == this->size());
    lue_hpx_assert(is_hypercube(shape));
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
    lue_hpx_assert(nr_elements(shape) > 0);
    lue_hpx_assert(this->shape()[0] == this->size());
    lue_hpx_assert(is_hypercube(shape));
}


template<
    typename Weight,
    Rank rank>
Kernel<Weight, rank>::Kernel(
    Array<Weight, rank> const& weights):

    Base{weights},
    _radius{(this->shape()[0] - 1) / 2}

{
    lue_hpx_assert(nr_elements(this->shape()) > 0);
    lue_hpx_assert(this->shape()[0] == this->size());
    lue_hpx_assert(is_hypercube(this->shape()));
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


template<
    typename Weight,
    Rank rank>
Kernel<Weight, rank> circle_kernel(
    Radius const radius,
    Weight const value)
{
    static_assert(rank <= 2);

    if constexpr(rank == 1)
    {
        return box_kernel<Weight, rank>(radius, value);
    }
    else {
        using Kernel = Kernel<Weight, rank>;
        using Shape = ShapeT<Kernel>;

        Shape shape{};
        std::fill(shape.begin(), shape.end(), 2 * radius + 1);

        Kernel kernel{shape, Weight{0}};

        // Assign the weight to all cells whose center is within a radius
        // distance of the kernel's centre
        Radius const distance_measure{radius * radius};

        {
            Radius dist0, dist1;

            for(Index idx0 = 0; idx0 < shape[0]; ++idx0)
            {
                dist0 = (idx0 - radius) * (idx0 - radius);

                for(Index idx1 = 0; idx1 < shape[1]; ++idx1)
                {
                    dist1 = (idx1 - radius) * (idx1 - radius);

                    if(dist0 + dist1 <= distance_measure)
                    {
                        kernel(idx0, idx1) = value;
                    }
                }
            }
        }

        return kernel;
    }
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
