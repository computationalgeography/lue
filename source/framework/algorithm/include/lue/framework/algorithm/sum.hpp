#pragma once
#include "lue/framework/algorithm/unary_aggregate_operation.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"


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
        OutputElement const input_element) const noexcept
    {
        return input_element;
    }

    constexpr OutputElement partition(
        OutputElement const aggregated_value,
        OutputElement const input_element) const noexcept
    {
        return aggregated_value + input_element;
    }

};

}  // namespace detail


namespace policy {
namespace sum {

template<
    typename OutputElement,
    typename InputElement>
using DefaultPolicies = policy::DefaultPolicies<
    OutputElements<OutputElement>,
    InputElements<InputElement>>;

}  // namespace sum
}  // namespace policy


template<
    typename InputElement,
    typename OutputElement=InputElement,
    Rank rank>
hpx::future<OutputElement> sum(
    PartitionedArray<InputElement, rank> const& array)
{
    using Functor = detail::Sum<InputElement, OutputElement>;
    using Policies = policy::sum::DefaultPolicies<OutputElement, InputElement>;

    return unary_aggregate_operation(Policies{}, array, Functor{});
}

}  // namespace lue
