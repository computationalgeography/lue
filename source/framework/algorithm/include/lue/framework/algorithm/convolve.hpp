#pragma once
#include "lue/framework/algorithm/focal_operation.hpp"


namespace lue {
namespace detail {

template<
    typename InputElement,
    typename OutputElement_>
class Convolve
{

public:

    using OutputElement = OutputElement_;

    static_assert(std::is_convertible_v<InputElement, OutputElement>);

    class Aggregator
    {

    public:

        Aggregator():
            _sum{0}
        {
        }

        template<
            typename Weight>
        void add(
            Weight const weight,
            InputElement const value)
        {
            static_assert(
                std::is_same_v<Weight, bool> ||
                std::is_floating_point_v<Weight>);

            if constexpr(std::is_same_v<Weight, bool>) {
                if(weight) {
                    _sum += value;
                }
            }
            else {
                _sum += weight * value;
            }
        }

        OutputElement operator()() const
        {
            return _sum;
        }

    private:

        OutputElement _sum;

    };

    Convolve()
    {
    }

    constexpr InputElement fill_value() const
    {
        return 0;
    }

};

}  // namespace detail


template<
    typename Array,
    typename Kernel,
    typename OutputElement=double>
PartitionedArrayT<Array, OutputElement> convolve(
    Array const& array,
    Kernel const& kernel)
{
    return focal_operation(
        array, kernel, detail::Convolve<ElementT<Array>, OutputElement>{});
}

}  // namespace lue
