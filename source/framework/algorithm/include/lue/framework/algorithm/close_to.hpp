#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"
#include <cmath>


namespace lue {
namespace detail {

// https://docs.scipy.org/doc/numpy/reference/generated/numpy.isclose.html
template<
    typename InputElement>
class CloseTo
{

public:

    static_assert(std::is_floating_point_v<InputElement>);

    using OutputElement = bool;

    CloseTo(
        InputElement const relative_difference=1e-5,
        InputElement const absolute_difference=1e-8):

        _relative_difference{relative_difference},
        _absolute_difference{absolute_difference}

    {
    }

    constexpr OutputElement operator()(
        InputElement const& input_element1,
        InputElement const& input_element2) const noexcept
    {
        return
            std::abs(input_element1 - input_element2) <= (
                _absolute_difference +
                _relative_difference * std::abs(input_element2)
            );
    }

private:

    InputElement _relative_difference;

    InputElement _absolute_difference;

};

}  // namespace detail


template<
    typename InputElement,
    Rank rank>
PartitionedArray<bool, rank> close_to(
    PartitionedArray<InputElement, rank> const& array1,
    PartitionedArray<InputElement, rank> const& array2)
{
    return binary_local_operation(
        array1, array2, detail::CloseTo<InputElement>{});
}


template<
    typename InputElement,
    Rank rank>
PartitionedArray<bool, rank> close_to(
    PartitionedArray<InputElement, rank> const& array,
    hpx::shared_future<InputElement> const& scalar)
{
    return binary_local_operation(
        array, scalar, detail::CloseTo<InputElement>{});
}


template<
    typename InputElement,
    Rank rank>
PartitionedArray<bool, rank> close_to(
    hpx::shared_future<InputElement> const& scalar,
    PartitionedArray<InputElement, rank> const& array)
{
    return binary_local_operation(
        scalar, array, detail::CloseTo<InputElement>{});
}


template<
    typename InputElement,
    Rank rank>
PartitionedArray<bool, rank> close_to(
    PartitionedArray<InputElement, rank> const& array,
    InputElement const scalar)
{
    return close_to(
        array, hpx::make_ready_future<InputElement>(scalar).share());
}


template<
    typename InputElement,
    Rank rank>
PartitionedArray<bool, rank> close_to(
    InputElement const scalar,
    PartitionedArray<InputElement, rank> const& array)
{
    return close_to(
        hpx::make_ready_future<InputElement>(scalar).share(), array);
}

}  // namespace lue
