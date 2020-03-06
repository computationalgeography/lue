#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"


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
    return greater_than_equal_to(
        array, hpx::make_ready_future<InputElement>(scalar).share());
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
    return greater_than_equal_to(
        hpx::make_ready_future<InputElement>(scalar).share(), array);
}

}  // namespace lue
