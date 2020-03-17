#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"


namespace lue {
namespace detail {

template<
    typename InputElement>
class Add
{

public:

    using OutputElement = InputElement;

    constexpr OutputElement operator()(
        InputElement const& input_element1,
        InputElement const& input_element2) const noexcept
    {
        return input_element1 + input_element2;
    }

};

}  // namespace detail


template<
    typename Element,
    Rank rank>
PartitionedArray<Element, rank> add(
    PartitionedArray<Element, rank> const& array1,
    PartitionedArray<Element, rank> const& array2)
{
    return binary_local_operation(array1, array2, detail::Add<Element>{});
}


template<
    typename Element,
    Rank rank>
PartitionedArray<Element, rank> add(
    PartitionedArray<Element, rank> const& array,
    hpx::shared_future<Element> const& scalar)
{
    return binary_local_operation(array, scalar, detail::Add<Element>{});
}


template<
    typename Element,
    Rank rank>
PartitionedArray<Element, rank> add(
    hpx::shared_future<Element> const& scalar,
    PartitionedArray<Element, rank> const& array)
{
    return binary_local_operation(scalar, array, detail::Add<Element>{});
}


template<
    typename Element,
    Rank rank>
PartitionedArray<Element, rank> add(
    PartitionedArray<Element, rank> const& array,
    Element const scalar)
{
    return add(array, hpx::make_ready_future<Element>(scalar).share());
}


template<
    typename Element,
    Rank rank>
PartitionedArray<Element, rank> add(
    Element const scalar,
    PartitionedArray<Element, rank> const& array)
{
    return add(hpx::make_ready_future<Element>(scalar).share(), array);
}

}  // namespace lue
