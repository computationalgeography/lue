#pragma once
#include "lue/framework/algorithm/unary_local_operation.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"
#include <cmath>
#include <limits>


namespace lue {
namespace detail {

template<
    typename InputElement>
class Sqrt
{

public:

    static_assert(std::is_floating_point_v<InputElement>);

    static_assert(std::numeric_limits<InputElement>::has_quiet_NaN);

    using OutputElement = InputElement;

    OutputElement operator()(
        InputElement const& input_element) const noexcept
    {
        // TODO(KDJ) Add support for a domain policy which handles this
        //     to unary local operation
        return input_element < 0
            ? std::numeric_limits<OutputElement>::quiet_NaN()
            : std::sqrt(input_element)
            ;
    }

};

}  // namespace detail


namespace policy {
namespace sqrt {

template<
    typename Element>
using DefaultPolicies = policy::DefaultPolicies<
    OutputElements<Element>,
    InputElements<Element>>;

}  // namespace sqrt
}  // namespace policy


template<
    typename Policies,
    typename Element,
    Rank rank>
PartitionedArray<Element, rank> sqrt(
    Policies const& policies,
    PartitionedArray<Element, rank> const& array)
{
    return unary_local_operation(policies, array, detail::Sqrt<Element>{});
}

template<
    typename Element,
    Rank rank>
PartitionedArray<Element, rank> sqrt(
    PartitionedArray<Element, rank> const& array)
{
    using Policies = policy::sqrt::DefaultPolicies<Element>;

    return sqrt(Policies{}, array);
}

}  // namespace lue
