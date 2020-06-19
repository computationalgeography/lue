#pragma once
#include "lue/framework/algorithm/unary_aggregate_operation.hpp"


namespace lue {
namespace detail {

template<
    typename InputElement,
    typename OutputElement_>
class Maximum
{

public:

    using OutputElement = OutputElement_;

    constexpr OutputElement operator()() const noexcept
    {
        // The result is small if there are no values to aggregate
        return std::numeric_limits<OutputElement>::min();
    }

    constexpr OutputElement operator()(
        InputElement const input_element) const noexcept
    {
        return input_element;
    }

    constexpr OutputElement operator()(
        InputElement const aggregated_value,
        InputElement const input_element) const noexcept
    {
        return std::max(aggregated_value, input_element);
    }

    constexpr OutputElement partition(
        InputElement const input_element) const noexcept
    {
        return input_element;
    }

    constexpr OutputElement partition(
        InputElement const aggregated_value,
        InputElement const input_element) const noexcept
    {
        return std::max(aggregated_value, input_element);
    }

};

}  // namespace detail


template<
    typename Element,
    Rank rank>
hpx::future<Element> maximum(
    PartitionedArray<Element, rank> const& array)
{
    return unary_aggregate_operation(array, detail::Maximum<Element, Element>{});
}

}  // namespace lue
