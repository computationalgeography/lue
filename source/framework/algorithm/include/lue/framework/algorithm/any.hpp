#pragma once
#include "lue/framework/algorithm/unary_aggregate_operation.hpp"


namespace lue {
namespace detail {

// Return true if any of the input elements is true
template<
    typename InputElement>
class Any
{

public:

    static_assert(std::is_convertible_v<InputElement, bool>);

    using OutputElement = InputElement;

    constexpr OutputElement operator()() const noexcept
    {
        // The result is false if there are not values to aggregate
        return false;
    }

    constexpr OutputElement operator()(
        InputElement const input_element) const noexcept
    {
        // The result is true if the input element is true
        return input_element;
    }

    constexpr OutputElement operator()(
        InputElement const aggregate_value,
        InputElement const input_element) const noexcept
    {
        // The result is true if the value aggregated until now is true
        // or the input element is true
        return aggregate_value || input_element;
    }

    constexpr OutputElement partition(
        InputElement const input_element) const noexcept
    {
        // The result is true if the input element is true
        return input_element;
    }

    constexpr OutputElement partition(
        InputElement const aggregated_value,
        InputElement const input_element) const noexcept
    {
        // The result is true if the value aggregated until now is true
        // or the input element is true
        return aggregated_value || input_element;
    }

};

}  // namespace detail


template<
    typename Element,
    Rank rank>
hpx::future<Element> any(
    PartitionedArray<Element, rank> const& array)
{
    return unary_aggregate_operation(array, detail::Any<Element>{});
}

}  // namespace lue
