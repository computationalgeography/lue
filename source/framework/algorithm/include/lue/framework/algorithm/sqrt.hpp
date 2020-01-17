#pragma once
#include "lue/framework/algorithm/unary_local_operation.hpp"
#include <cmath>
#include <limits>


namespace lue {
namespace detail {

template<
    typename InputElement>
class Sqrt
{

public:

    static_assert(std::is_floating_point_v<InputElement>);

    static_assert(std::numeric_limits<InputElement>::has_quiet_NaN);

    using OutputElement = InputElement;

    OutputElement operator()(
        InputElement const& input_element) const
    {
        return input_element < 0
            ? std::numeric_limits<OutputElement>::quiet_NaN()
            : std::sqrt(input_element)
            ;
    }

};

}  // namespace detail


template<
    typename Array>
Array sqrt(
    Array const& array)
{
    return unary_local_operation(array, detail::Sqrt<ElementT<Array>>{});
}

}  // namespace lue
