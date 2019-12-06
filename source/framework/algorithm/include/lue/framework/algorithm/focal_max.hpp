#pragma once
#include "lue/framework/algorithm/focal_operation.hpp"
#include <limits>


namespace lue {
namespace detail {

template<
    typename InputElement>
class FocalMax
{

public:

    using OutputElement = InputElement;

    class Aggregator
    {

    public:

        Aggregator():
            _max{0},
            _initialized{false}
        {
        }

        template<
            typename Weight>
        void add(
            Weight const weight,
            InputElement const value)
        {
            static_assert(std::is_same_v<Weight, bool>);

            if(weight) {
                if(!_initialized) {
                    _max = value;
                    _initialized = true;
                }
                else {
                    _max = std::max(_max, value);
                }
            }
        }

        OutputElement operator()() const
        {
            return _max;
        }

    private:

        OutputElement _max;

        bool _initialized;

    };

    FocalMax()
    {
    }

    constexpr InputElement fill_value() const
    {
        return std::numeric_limits<InputElement>::min();
    }

};

}  // namespace detail


// focal_max: given
// - a partitioned array
//     - Element type is not relevant. As long as the values support
//       being compared using operator<.
// - a kernel
//     - Element type is not relevant. As long as the values support
//       being evaluated as boolean.
// iterate kernel over array and store max value found in kernel window
// in focal cell

// Implementation calls a more generic algorithm accepting a partitioned
// array, a kernel and a functor: focal operation
// This algorithm handles the halo partitions around the array, the
// partitioning of the algorithm, and calling the functor, passing in
// the kernel and the (view on the) elements

template<
    typename Array,
    typename Kernel>
Array focal_max(
    Array const& array,
    Kernel const& kernel)
{
    return focal_operation(array, kernel, detail::FocalMax<ElementT<Array>>{});
}

}  // namespace lue
