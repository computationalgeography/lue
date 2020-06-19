#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"


namespace lue {
namespace detail {

template<
    typename InputElement>
class LogicalInclusiveOr
{

public:

    using OutputElement = bool;

    constexpr OutputElement operator()(
        InputElement const& input_element1,
        InputElement const& input_element2) const noexcept
    {
        return input_element1 || input_element2;
    }

};

}  // namespace detail


template<
    typename InputElement,
    Rank rank>
PartitionedArray<bool, rank> logical_inclusive_or(
    PartitionedArray<InputElement, rank> const& array1,
    PartitionedArray<InputElement, rank> const& array2)
{
    return binary_local_operation(array1, array2, detail::LogicalInclusiveOr<InputElement>{});
}


template<
    typename InputElement,
    Rank rank>
PartitionedArray<bool, rank> logical_inclusive_or(
    PartitionedArray<InputElement, rank> const& array,
    hpx::shared_future<InputElement> const& scalar)
{
    return binary_local_operation(array, scalar, detail::LogicalInclusiveOr<InputElement>{});
}


template<
    typename InputElement,
    Rank rank>
PartitionedArray<bool, rank> logical_inclusive_or(
    hpx::shared_future<InputElement> const& scalar,
    PartitionedArray<InputElement, rank> const& array)
{
    return binary_local_operation(scalar, array, detail::LogicalInclusiveOr<InputElement>{});
}


template<
    typename InputElement,
    Rank rank>
PartitionedArray<bool, rank> logical_inclusive_or(
    PartitionedArray<InputElement, rank> const& array,
    InputElement const& scalar)
{
    return logical_inclusive_or(array, hpx::make_ready_future<InputElement>(scalar).share());
}


template<
    typename InputElement,
    Rank rank>
PartitionedArray<bool, rank> logical_inclusive_or(
    InputElement const& scalar,
    PartitionedArray<InputElement, rank> const& array)
{
    return logical_inclusive_or(hpx::make_ready_future<InputElement>(scalar).share(), array);
}

}  // namespace lue
