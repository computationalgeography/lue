#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"


namespace lue {
namespace detail {

template<
    typename InputElement>
class GreaterEqual
{

public:

    using OutputElement = bool;

    OutputElement operator()(
        InputElement const& input_element1,
        InputElement const& input_element2) const
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
    Rank rank,
    template<typename, Rank> typename Array>
Array<bool, rank> greater_equal(
    Array<InputElement, rank> const& array1,
    Array<InputElement, rank> const& array2)
{
    return binary_local_operation(
        array1, array2, detail::GreaterEqual<InputElement>{});
}


/*!
    @overload
*/
template<
    typename InputElement,
    Rank rank,
    template<typename, Rank> typename Array>
Array<bool, rank> greater_equal(
    Array<InputElement, rank> const& array,
    hpx::shared_future<InputElement> const& scalar)
{
    return binary_local_operation(
        array, scalar, detail::GreaterEqual<InputElement>{});
}


/*!
    @overload
*/
template<
    typename InputElement,
    Rank rank,
    template<typename, Rank> typename Array>
Array<bool, rank> greater_equal(
    hpx::shared_future<InputElement> const& scalar,
    Array<InputElement, rank> const& array)
{
    return binary_local_operation(
        scalar, array, detail::GreaterEqual<InputElement>{});
}

}  // namespace lue
