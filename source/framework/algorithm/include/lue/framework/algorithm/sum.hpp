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

using DefaultPolicies = policy::DefaultPolicies<1, 1>;

}  // namespace policy
}  // namespace sum


template<
    typename InputElement,
    typename OutputElement=InputElement,
    Rank rank>
hpx::future<OutputElement> sum(
    PartitionedArray<InputElement, rank> const& array)
{
    return unary_aggregate_operation(
        policy::sum::DefaultPolicies{},
        array, detail::Sum<InputElement, OutputElement>{});
}

}  // namespace lue
