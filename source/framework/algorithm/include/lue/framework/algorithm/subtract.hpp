#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"


namespace lue {
namespace detail {

template<
    typename InputElement>
class Subtract
{

public:

    using OutputElement = InputElement;

    constexpr OutputElement operator()(
        InputElement const& input_element1,
        InputElement const& input_element2) const noexcept
    {
        return input_element1 - input_element2;
    }

};

}  // namespace detail


template<
    typename Element,
    Rank rank>
PartitionedArray<Element, rank> subtract(
    PartitionedArray<Element, rank> const& array1,
    PartitionedArray<Element, rank> const& array2)
{
    return binary_local_operation(array1, array2, detail::Subtract<Element>{});
}


template<
    typename Element,
    Rank rank>
PartitionedArray<Element, rank> subtract(
    PartitionedArray<Element, rank> const& array,
    hpx::shared_future<Element> const& scalar)
{
    return binary_local_operation(array, scalar, detail::Subtract<Element>{});
}


template<
    typename Element,
    Rank rank>
PartitionedArray<Element, rank> subtract(
    hpx::shared_future<Element> const& scalar,
    PartitionedArray<Element, rank> const& array)
{
    return binary_local_operation(scalar, array, detail::Subtract<Element>{});
}

}  // namespace lue
