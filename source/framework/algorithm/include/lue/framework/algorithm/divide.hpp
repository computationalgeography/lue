#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"


namespace lue {
namespace detail {

template<
    typename InputElement>
class Divide
{

public:

    static_assert(std::is_floating_point_v<InputElement>);

    static_assert(std::numeric_limits<InputElement>::has_quiet_NaN);

    using OutputElement = InputElement;

    constexpr OutputElement operator()(
        InputElement const& input_element1,
        InputElement const& input_element2) const noexcept
    {
        // TODO(KDJ) Move this test to a domain policy used by the binary
        //     local operation
        return input_element2 == 0
            ? std::numeric_limits<OutputElement>::quiet_NaN()
            : input_element1 / input_element2
            ;
    }

};

}  // namespace detail


namespace policy {
namespace divide {

using DefaultPolicies = policy::DefaultPolicies<2, 1>;

}  // namespace policy
}  // namespace divide


template<
    typename Element,
    Rank rank>
PartitionedArray<Element, rank> divide(
    PartitionedArray<Element, rank> const& array1,
    PartitionedArray<Element, rank> const& array2)
{
    return binary_local_operation(
        policy::divide::DefaultPolicies{},
        array1, array2,
        detail::Divide<Element>{});
}


template<
    typename Element,
    Rank rank>
PartitionedArray<Element, rank> divide(
    PartitionedArray<Element, rank> const& array,
    hpx::shared_future<Element> const& scalar)
{
    return binary_local_operation(
        policy::divide::DefaultPolicies{},
        array, scalar,
        detail::Divide<Element>{});
}


template<
    typename Element,
    Rank rank>
PartitionedArray<Element, rank> divide(
    hpx::shared_future<Element> const& scalar,
    PartitionedArray<Element, rank> const& array)
{
    return binary_local_operation(
        policy::divide::DefaultPolicies{},
        scalar, array,
        detail::Divide<Element>{});
}


template<
    typename Element,
    Rank rank>
PartitionedArray<Element, rank> divide(
    PartitionedArray<Element, rank> const& array,
    Element const& scalar)
{
    return binary_local_operation(
        policy::divide::DefaultPolicies{},
        array, hpx::make_ready_future<Element>(scalar).share(),
        detail::Divide<Element>{});
}


template<
    typename Element,
    Rank rank>
PartitionedArray<Element, rank> divide(
    Element const& scalar,
    PartitionedArray<Element, rank> const& array)
{
    return binary_local_operation(
        policy::divide::DefaultPolicies{},
        hpx::make_ready_future<Element>(scalar).share(), array,
        detail::Divide<Element>{});
}

}  // namespace lue
