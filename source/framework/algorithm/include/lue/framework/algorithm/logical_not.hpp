#pragma once
#include "lue/framework/algorithm/unary_local_operation.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"


namespace lue {
namespace detail {

template<
    typename InputElement>
class LogicalNot
{

public:

    using OutputElement = bool;

    constexpr OutputElement operator()(
        InputElement const& input_element) const noexcept
    {
        return !input_element;
    }

};

}  // namespace detail


namespace policy {
namespace logical_not {

template<
    typename OutputElement,
    typename InputElement>
using DefaultPolicies = policy::DefaultPolicies<
    OutputElements<OutputElement>,
    InputElements<InputElement>>;

}  // namespace logical_not
}  // namespace policy


template<
    typename InputElement,
    Rank rank>
PartitionedArray<bool, rank> logical_not(
    PartitionedArray<InputElement, rank> const& array)
{
    using Functor = detail::LogicalNot<InputElement>;
    using OutputElement = OutputElementT<Functor>;
    using Policies = policy::logical_not::DefaultPolicies<OutputElement, InputElement>;

    return unary_local_operation(Policies{}, array, Functor{});
}

}  // namespace lue
