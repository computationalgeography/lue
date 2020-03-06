#pragma once
#include "lue/framework/algorithm/unary_aggregate_operation.hpp"


namespace lue {
namespace detail {

template<
    typename InputElement,
    typename OutputElement_>
class Sum
{

public:

    using OutputElement = OutputElement_;

    constexpr OutputElement operator()() const noexcept
    {
        // The result is zero if there are no values to aggregate
        return 0;
    }

    constexpr OutputElement operator()(
        InputElement const input_element) const noexcept
    {
        return input_element;
    }

    constexpr OutputElement operator()(
        InputElement const aggregate_value,
        InputElement const input_element) const noexcept
    {
        return aggregate_value + input_element;
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
        return aggregated_value + input_element;
    }

};

}  // namespace detail


template<
    typename InputElement,
    typename OutputElement=InputElement,
    Rank rank>
hpx::future<OutputElement> sum(
    PartitionedArray<InputElement, rank> const& array)
{
    return unary_aggregate_operation(
        array, detail::Sum<InputElement, OutputElement>{});
}

}  // namespace lue
