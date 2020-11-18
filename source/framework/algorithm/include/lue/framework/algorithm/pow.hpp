#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"
#include <cmath>
#include <limits>


namespace lue {
namespace detail {

template<
    typename InputElement>
class Pow
{

public:

    static_assert(std::is_floating_point_v<InputElement>);

    /// static_assert(std::numeric_limits<InputElement>::has_quiet_NaN);

    using OutputElement = InputElement;

    OutputElement operator()(
        InputElement const input_element,
        InputElement const exponent) const noexcept
    {
        return std::pow(input_element, exponent);
    }

};

}  // namespace detail


namespace policy {
namespace pow {

using DefaultPolicies = policy::DefaultPolicies<2, 1>;

}  // namespace policy
}  // namespace pow


template<
    typename Element,
    Rank rank>
PartitionedArray<Element, rank> pow(
    PartitionedArray<Element, rank> const& array,
    hpx::shared_future<Element> const& exponent)
{
    return binary_local_operation(
        policy::pow::DefaultPolicies{},
        array, exponent,
        detail::Pow<Element>{});
}


template<
    typename Element,
    Rank rank>
PartitionedArray<Element, rank> pow(
    PartitionedArray<Element, rank> const& array,
    Element const exponent)
{
    return pow(array, hpx::make_ready_future<Element>(exponent).share());
}

}  // namespace lue
