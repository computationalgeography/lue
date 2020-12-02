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

using DefaultPolicies = policy::DefaultPolicies<1, 1>;

}  // namespace logical_not
}  // namespace policy


template<
    typename InputElement,
    Rank rank>
PartitionedArray<bool, rank> logical_not(
    PartitionedArray<InputElement, rank> const& array)
{
    return unary_local_operation(
        policy::logical_not::DefaultPolicies{},
        array,
        detail::LogicalNot<InputElement>{});
}

}  // namespace lue
