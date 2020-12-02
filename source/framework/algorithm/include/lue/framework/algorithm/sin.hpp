#pragma once
#include "lue/framework/algorithm/unary_local_operation.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"
#include <cmath>


namespace lue {
namespace detail {

template<
    typename InputElement>
class Sin
{

public:

    static_assert(std::is_floating_point_v<InputElement>);

    using OutputElement = InputElement;

    OutputElement operator()(
        InputElement const& input_element) const noexcept
    {
        return std::sin(input_element);
    }

};

}  // namespace detail


namespace policy {
namespace sin {

using DefaultPolicies = policy::DefaultPolicies<1, 1>;

}  // namespace sin
}  // namespace policy


template<
    typename Element,
    Rank rank>
PartitionedArray<Element, rank> sin(
    PartitionedArray<Element, rank> const& array)
{
    return unary_local_operation(
        policy::sin::DefaultPolicies{},
        array,
        detail::Sin<Element>{});
}

}  // namespace lue
