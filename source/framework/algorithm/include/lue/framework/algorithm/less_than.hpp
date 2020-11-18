#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"
#include "lue/framework/algorithm/operator.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"


namespace lue {
namespace detail {

template<
    typename InputElement>
class LessThan
{

public:

    using OutputElement = bool;

    constexpr OutputElement operator()(
        InputElement const& input_element1,
        InputElement const& input_element2) const noexcept
    {
        return input_element1 < input_element2;
    }

};

}  // namespace detail


namespace policy {
namespace less_than {

using DefaultPolicies = policy::DefaultPolicies<2, 1>;

}  // namespace policy
}  // namespace less_than


template<
    typename InputElement,
    Rank rank>
PartitionedArray<bool, rank> less_than(
    PartitionedArray<InputElement, rank> const& array1,
    PartitionedArray<InputElement, rank> const& array2)
{
    return binary_local_operation(
        policy::less_than::DefaultPolicies{},
        array1, array2,
        detail::LessThan<InputElement>{});
}


template<
    typename InputElement,
    Rank rank>
PartitionedArray<bool, rank> less_than(
    PartitionedArray<InputElement, rank> const& array,
    hpx::shared_future<InputElement> const& scalar)
{
    return binary_local_operation(
        policy::less_than::DefaultPolicies{},
        array, scalar, detail::LessThan<InputElement>{});
}


template<
    typename InputElement,
    Rank rank>
PartitionedArray<bool, rank> less_than(
    hpx::shared_future<InputElement> const& scalar,
    PartitionedArray<InputElement, rank> const& array)
{
    return binary_local_operation(
        policy::less_than::DefaultPolicies{},
        scalar, array,
        detail::LessThan<InputElement>{});
}


template<
    typename InputElement,
    Rank rank>
PartitionedArray<bool, rank> less_than(
    PartitionedArray<InputElement, rank> const& array,
    InputElement const& scalar)
{
    return less_than(array, hpx::make_ready_future<InputElement>(scalar).share());
}


template<
    typename InputElement,
    Rank rank>
PartitionedArray<bool, rank> less_than(
    InputElement const& scalar,
    PartitionedArray<InputElement, rank> const& array)
{
    return less_than(hpx::make_ready_future<InputElement>(scalar).share(), array);
}


LUE_BINARY_COMPARISON_OPERATOR(<, less_than)

}  // namespace lue
