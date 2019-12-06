#pragma once
#include "lue/framework/algorithm/focal_operation.hpp"
#include <cmath>
#include <limits>


namespace lue {
namespace detail {

template<
    typename InputElement>
class FocalMean
{

public:

    // If not, an OutputElement type must be passed in
    static_assert(std::is_floating_point_v<InputElement>);

    static_assert(std::numeric_limits<InputElement>::has_quiet_NaN);

    using OutputElement = InputElement;

    class Aggregator
    {

    public:

        Aggregator():
            _sum{0},
            _count{0}
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
                if(weight && !std::isnan(value)) {
                    _sum += value;
                    ++_count;
                }
            }
            else {
                if(!std::isnan(value)) {
                    _sum += weight * value;
                    ++_count;
                }
            }
        }

        OutputElement operator()() const
        {
            return _count > 0
                ? _sum / _count
                : std::numeric_limits<InputElement>::quiet_NaN();
        }

    private:

        OutputElement _sum;

        Count _count;

    };

    FocalMean()
    {
    }

    constexpr InputElement fill_value() const
    {
        return std::numeric_limits<InputElement>::quiet_NaN();
    }

};

}  // namespace detail


template<
    typename Array,
    typename Kernel>
Array focal_mean(
    Array const& array,
    Kernel const& kernel)
{
    return focal_operation(
        array, kernel, detail::FocalMean<ElementT<Array>>{});
}

}  // namespace lue
