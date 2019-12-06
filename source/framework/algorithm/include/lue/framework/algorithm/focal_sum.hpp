#pragma once
#include "lue/framework/algorithm/focal_operation.hpp"


namespace lue {
namespace detail {

template<
    typename InputElement>
class FocalSum
{

public:

    using OutputElement = InputElement;

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

    FocalSum()
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
    typename Kernel>
Array focal_sum(
    Array const& array,
    Kernel const& kernel)
{
    return focal_operation(array, kernel, detail::FocalSum<ElementT<Array>>{});
}

}  // namespace lue
