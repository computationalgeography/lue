#pragma once
#include "lue/framework/algorithm/unary_local_operation.hpp"


namespace lue {
namespace detail {

template<
    typename InputElement>
class IteratePerElement
{

public:

    static_assert(std::is_integral_v<InputElement>);

    using OutputElement = InputElement;

    OutputElement operator()(
        InputElement const& input_element) const
    {
        // The use of volatile prevends the optimizing compiler
        // to remove this iteration
        volatile InputElement nr_iterations = input_element;
        assert(nr_iterations >= InputElement{0});

        while(nr_iterations > InputElement{0}) {
            --nr_iterations;
        }

        return input_element;
    }

};

}  // namespace detail


/*!
    @brief      Per cell in a partitioned array, iterate a number of
                times before copying the cell to the result
    @tparam     Array Type of the input and output partitioned arrays
    @param      array Input partitioned array
    @return     New partitioned array

    This algorithm has the same effect as the copy algorithm, but spends
    some more time. It is useful when simulating a configurable amount
    of load per cell.
*/
template<
    typename Array>
Array iterate_per_element(
    Array const& array)
{
    return unary_local_operation(
        array, detail::IteratePerElement<ElementT<Array>>{});
}

}  // namespace lue
