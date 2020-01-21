#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"


namespace lue {
namespace detail {

template<
    typename InputElement>
class Multiply
{

public:

    using OutputElement = InputElement;

    OutputElement operator()(
        InputElement const& input_element1,
        InputElement const& input_element2) const
    {
        return input_element1 * input_element2;
    }

};

}  // namespace detail


template<
    typename Element,
    Rank rank,
    template<typename, Rank> typename Array>
Array<Element, rank> multiply(
    Array<Element, rank> const& array1,
    Array<Element, rank> const& array2)
{
    return binary_local_operation(
        array1, array2, detail::Multiply<Element>{});
}


template<
    typename Element,
    Rank rank,
    template<typename, Rank> typename Array>
Array<Element, rank> multiply(
    Array<Element, rank> const& array,
    hpx::shared_future<Element> const& scalar)
{
    return binary_local_operation(
        array, scalar, detail::Multiply<Element>{});
}


template<
    typename Element,
    Rank rank,
    template<typename, Rank> typename Array>
Array<Element, rank> multiply(
    hpx::shared_future<Element> const& scalar,
    Array<Element, rank> const& array)
{
    return binary_local_operation(
        scalar, array, detail::Multiply<Element>{});
}

}  // namespace lue
