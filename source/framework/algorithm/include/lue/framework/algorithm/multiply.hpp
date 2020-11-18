#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"


namespace lue {
namespace detail {

template<
    typename InputElement>
class Multiply
{

public:

    using OutputElement = InputElement;

    constexpr OutputElement operator()(
        InputElement const& input_element1,
        InputElement const& input_element2) const noexcept
    {
        return input_element1 * input_element2;
    }

};

}  // namespace detail


namespace policy {
namespace multiply {

using DefaultPolicies = policy::DefaultPolicies<2, 1>;

}  // namespace policy
}  // namespace multiply


template<
    typename Element,
    Rank rank>
PartitionedArray<Element, rank> multiply(
    PartitionedArray<Element, rank> const& array1,
    PartitionedArray<Element, rank> const& array2)
{
    return binary_local_operation(
        policy::multiply::DefaultPolicies{},
        array1, array2,
        detail::Multiply<Element>{});
}


template<
    typename Element,
    Rank rank>
PartitionedArray<Element, rank> multiply(
    PartitionedArray<Element, rank> const& array,
    hpx::shared_future<Element> const& scalar)
{
    return binary_local_operation(
        policy::multiply::DefaultPolicies{},
        array, scalar,
        detail::Multiply<Element>{});
}


template<
    typename Element,
    Rank rank>
PartitionedArray<Element, rank> multiply(
    hpx::shared_future<Element> const& scalar,
    PartitionedArray<Element, rank> const& array)
{
    return binary_local_operation(
        policy::multiply::DefaultPolicies{},
        scalar, array,
        detail::Multiply<Element>{});
}


template<
    typename Element,
    Rank rank>
PartitionedArray<Element, rank> multiply(
    PartitionedArray<Element, rank> const& array,
    Element const& scalar)
{
    return binary_local_operation(
        policy::multiply::DefaultPolicies{},
        array, hpx::make_ready_future<Element>(scalar).share(),
        detail::Multiply<Element>{});
}


template<
    typename Element,
    Rank rank>
PartitionedArray<Element, rank> multiply(
    Element const& scalar,
    PartitionedArray<Element, rank> const& array)
{
    return binary_local_operation(
        policy::multiply::DefaultPolicies{},
        hpx::make_ready_future<Element>(scalar).share(), array,
        detail::Multiply<Element>{});
}

}  // namespace lue
