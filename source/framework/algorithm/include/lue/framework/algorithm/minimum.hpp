#pragma once
#include "lue/framework/algorithm/unary_aggregate_operation.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"


namespace lue {
namespace detail {

template<
    typename InputElement,
    typename OutputElement_>
class Minimum
{

public:

    using OutputElement = OutputElement_;

    constexpr OutputElement operator()() const noexcept
    {
        // The result is large if there are no values to aggregate
        return std::numeric_limits<OutputElement>::max();
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
        return std::min(aggregated_value, input_element);
    }

    constexpr OutputElement partition(
        OutputElement const input_element) const noexcept
    {
        return input_element;
    }

    constexpr OutputElement partition(
        OutputElement const aggregated_value,
        OutputElement const input_element) const noexcept
    {
        return std::min(aggregated_value, input_element);
    }

};

}  // namespace detail


namespace policy {
namespace minimum {

template<
    typename Element>
using DefaultPolicies = policy::DefaultPolicies<
    OutputElements<Element>,
    InputElements<Element>>;

}  // namespace minimum
}  // namespace policy


template<
    typename Element,
    Rank rank>
hpx::future<Element> minimum(
    PartitionedArray<Element, rank> const& array)
{
    using Functor = detail::Minimum<Element, Element>;
    using Policies = policy::minimum::DefaultPolicies<Element>;

    return unary_aggregate_operation(Policies{}, array, Functor{});
}

}  // namespace lue
