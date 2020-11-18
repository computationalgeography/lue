#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"
#include "lue/framework/algorithm/operator.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"


namespace lue {
namespace detail {

template<
    typename InputElement>
class GreaterThanEqualTo
{

public:

    using OutputElement = bool;

    constexpr OutputElement operator()(
        InputElement const& input_element1,
        InputElement const& input_element2) const noexcept
    {
        return input_element1 >= input_element2;
    }

};

}  // namespace detail


namespace policy {
namespace greater_than_equal_to {

using DefaultPolicies = policy::DefaultPolicies<2, 1>;

}  // namespace policy
}  // namespace greater_than_equal_to


/*!
    @brief      Return the result of comparing two partitioned arrays
    @param      array1 Partitioned array
    @param      array2 Partitioned array
    @return     New partitioned array
*/
template<
    typename InputElement,
    Rank rank>
PartitionedArray<bool, rank> greater_than_equal_to(
    PartitionedArray<InputElement, rank> const& array1,
    PartitionedArray<InputElement, rank> const& array2)
{
    return binary_local_operation(
        policy::greater_than_equal_to::DefaultPolicies{},
        array1, array2, detail::GreaterThanEqualTo<InputElement>{});
}


/*!
    @overload
*/
template<
    typename InputElement,
    Rank rank>
PartitionedArray<bool, rank> greater_than_equal_to(
    PartitionedArray<InputElement, rank> const& array,
    hpx::shared_future<InputElement> const& scalar)
{
    return binary_local_operation(
        policy::greater_than_equal_to::DefaultPolicies{},
        array, scalar, detail::GreaterThanEqualTo<InputElement>{});
}


/*!
    @overload
*/
template<
    typename InputElement,
    Rank rank>
PartitionedArray<bool, rank> greater_than_equal_to(
    hpx::shared_future<InputElement> const& scalar,
    PartitionedArray<InputElement, rank> const& array)
{
    return binary_local_operation(
        policy::greater_than_equal_to::DefaultPolicies{},
        scalar, array, detail::GreaterThanEqualTo<InputElement>{});
}


/*!
    @overload
*/
template<
    typename InputElement,
    Rank rank>
PartitionedArray<bool, rank> greater_than_equal_to(
    PartitionedArray<InputElement, rank> const& array,
    InputElement const& scalar)
{
    return greater_than_equal_to(array, hpx::make_ready_future<InputElement>(scalar).share());
}


/*!
    @overload
*/
template<
    typename InputElement,
    Rank rank>
PartitionedArray<bool, rank> greater_than_equal_to(
    InputElement const& scalar,
    PartitionedArray<InputElement, rank> const& array)
{
    return greater_than_equal_to(hpx::make_ready_future<InputElement>(scalar).share(), array);
}


LUE_BINARY_COMPARISON_OPERATOR(>=, greater_than_equal_to)

}  // namespace lue
