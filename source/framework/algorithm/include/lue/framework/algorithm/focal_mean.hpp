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
            _sum_of_weights{0}
        {
        }

        template<
            typename Weight>
        void add(
            Weight const weight,
            InputElement const value)
        {
            static_assert(std::is_same_v<Weight, bool> || std::is_floating_point_v<Weight>);

            if constexpr(std::is_same_v<Weight, bool>) {
                if(weight && !std::isnan(value)) {
                    _sum += value;
                    _sum_of_weights += 1.0;
                }
            }
            else {
                if(!std::isnan(value)) {
                    _sum += weight * value;
                    _sum_of_weights += weight;
                }
            }
        }

        OutputElement operator()() const
        {
            return _sum_of_weights > 0.0
                ? _sum / _sum_of_weights
                : std::numeric_limits<InputElement>::quiet_NaN();
        }

    private:

        OutputElement _sum;

        double _sum_of_weights;

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
    typename Element,
    Rank rank,
    typename Kernel>
PartitionedArray<Element, rank> focal_mean(
    PartitionedArray<Element, rank> const& array,
    Kernel const& kernel)
{
    return focal_operation(array, kernel, detail::FocalMean<Element>{});
}

}  // namespace lue
