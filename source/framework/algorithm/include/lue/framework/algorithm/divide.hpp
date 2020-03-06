#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"


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
        return input_element2 == 0
            ? std::numeric_limits<OutputElement>::quiet_NaN()
            : input_element1 / input_element2
            ;
    }

};

}  // namespace detail


template<
    typename Element,
    Rank rank>
PartitionedArray<Element, rank> divide(
    PartitionedArray<Element, rank> const& array1,
    PartitionedArray<Element, rank> const& array2)
{
    return binary_local_operation(
        array1, array2, detail::Divide<Element>{});
}


template<
    typename Element,
    Rank rank>
PartitionedArray<Element, rank> divide(
    PartitionedArray<Element, rank> const& array,
    hpx::shared_future<Element> const& scalar)
{
    return binary_local_operation(
        array, scalar, detail::Divide<Element>{});
}


template<
    typename Element,
    Rank rank>
PartitionedArray<Element, rank> divide(
    hpx::shared_future<Element> const& scalar,
    PartitionedArray<Element, rank> const& array)
{
    return binary_local_operation(
        scalar, array, detail::Divide<Element>{});
}


template<
    typename Element,
    Rank rank>
PartitionedArray<Element, rank> divide(
    PartitionedArray<Element, rank> const& array,
    Element const& scalar)
{
    return binary_local_operation(
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
        hpx::make_ready_future<Element>(scalar).share(),
        array, detail::Divide<Element>{});
}

}  // namespace lue
