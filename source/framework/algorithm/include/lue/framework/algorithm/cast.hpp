#pragma once
#include "lue/framework/algorithm/unary_local_operation.hpp"


namespace lue {
namespace detail {

template<
    typename InputElement,
    typename OutputElement_>
class Cast
{

public:

    using OutputElement = OutputElement_;

    OutputElement operator()(
        InputElement const& input_element) const
    {
        return static_cast<OutputElement>(input_element);
    }

};

}  // namespace detail


template<
    typename OutputElement,
    typename InputArray>
PartitionedArrayT<InputArray, OutputElement> cast(
    InputArray const& array)
{
    return unary_local_operation(
        array, detail::Cast<ElementT<InputArray>, OutputElement>{});
}

}  // namespace lue
