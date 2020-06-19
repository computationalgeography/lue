#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"
#include "lue/framework/algorithm/operator.hpp"


namespace lue {
namespace detail {

template<
    typename InputElement>
class EqualTo
{

public:

    using OutputElement = bool;

    constexpr OutputElement operator()(
        InputElement const& input_element1,
        InputElement const& input_element2) const noexcept
    {
        return input_element1 == input_element2;
    }

};

}  // namespace detail


// array == array
template<
    typename InputElement,
    Rank rank>
PartitionedArray<bool, rank> equal_to(
    PartitionedArray<InputElement, rank> const& array1,
    PartitionedArray<InputElement, rank> const& array2)
{
    return binary_local_operation(array1, array2, detail::EqualTo<InputElement>{});
}


// array == scalar
template<
    typename InputElement,
    Rank rank>
PartitionedArray<bool, rank> equal_to(
    PartitionedArray<InputElement, rank> const& array,
    hpx::shared_future<InputElement> const& scalar)
{
    return binary_local_operation(array, scalar, detail::EqualTo<InputElement>{});
}


// scalar == array
template<
    typename InputElement,
    Rank rank>
PartitionedArray<bool, rank> equal_to(
    hpx::shared_future<InputElement> const& scalar,
    PartitionedArray<InputElement, rank> const& array)
{
    return binary_local_operation(scalar, array, detail::EqualTo<InputElement>{});
}


// array == scalar
template<
    typename InputElement,
    Rank rank>
PartitionedArray<bool, rank> equal_to(
    PartitionedArray<InputElement, rank> const& array,
    InputElement const& scalar)
{
    return equal_to(array, hpx::make_ready_future<InputElement>(scalar).share());
}


// scalar == array
template<
    typename InputElement,
    Rank rank>
PartitionedArray<bool, rank> equal_to(
    InputElement const& scalar,
    PartitionedArray<InputElement, rank> const& array)
{
    return equal_to(hpx::make_ready_future<InputElement>(scalar).share(), array);
}


// partition == scalar
template<
    typename InputElement,
    Rank rank>
ArrayPartition<bool, rank> equal_to(
    hpx::id_type const locality_id,
    ArrayPartition<InputElement, rank> const& partition,
    hpx::shared_future<InputElement> const& scalar)
{
    return binary_local_operation(locality_id, partition, scalar, detail::EqualTo<InputElement>{});
}


// partition == scalar
template<
    typename InputElement,
    Rank rank>
ArrayPartition<bool, rank> equal_to(
    hpx::id_type const locality_id,
    ArrayPartition<InputElement, rank> const& partition,
    InputElement const& scalar)
{
    return equal_to(locality_id, partition, hpx::make_ready_future<InputElement>(scalar).share());
}


LUE_BINARY_COMPARISON_OPERATOR(==, equal_to)

}  // namespace lue
