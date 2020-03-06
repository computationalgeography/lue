#pragma once
#include "lue/framework/algorithm/unary_local_operation.hpp"


namespace lue {
namespace detail {

template<
    typename InputElement>
class LogicalNot
{

public:

    using OutputElement = bool;

    constexpr OutputElement operator()(
        InputElement const& input_element) const noexcept
    {
        return !input_element;
    }

};

}  // namespace detail


template<
    typename InputElement,
    Rank rank>
PartitionedArray<bool, rank> logical_not(
    PartitionedArray<InputElement, rank> const& array)
{
    return unary_local_operation(array, detail::LogicalNot<InputElement>{});
}

}  // namespace lue
