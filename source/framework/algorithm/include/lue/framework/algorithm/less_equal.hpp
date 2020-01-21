#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"


namespace lue {
namespace detail {

template<
    typename InputElement>
class LessEqual
{

public:

    using OutputElement = bool;

    OutputElement operator()(
        InputElement const& input_element1,
        InputElement const& input_element2) const
    {
        return input_element1 <= input_element2;
    }

};

}  // namespace detail


template<
    typename InputElement,
    Rank rank,
    template<typename, Rank> typename Array>
Array<bool, rank> less_equal(
    Array<InputElement, rank> const& array1,
    Array<InputElement, rank> const& array2)
{
    return binary_local_operation(
        array1, array2, detail::LessEqual<InputElement>{});
}


template<
    typename InputElement,
    Rank rank,
    template<typename, Rank> typename Array>
Array<bool, rank> less_equal(
    Array<InputElement, rank> const& array,
    hpx::shared_future<InputElement> const& scalar)
{
    return binary_local_operation(
        array, scalar, detail::LessEqual<InputElement>{});
}


template<
    typename InputElement,
    Rank rank,
    template<typename, Rank> typename Array>
Array<bool, rank> less_equal(
    hpx::shared_future<InputElement> const& scalar,
    Array<InputElement, rank> const& array)
{
    return binary_local_operation(
        scalar, array, detail::LessEqual<InputElement>{});
}

}  // namespace lue
