#pragma once
#include "lue/framework/algorithm/unary_local_operation.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"


namespace lue {
namespace detail {

template<
    typename InputElement,
    typename OutputElement_>
class Cast
{

public:

    using OutputElement = OutputElement_;

    constexpr OutputElement operator()(
        InputElement const& input_element) const noexcept
    {
        return static_cast<OutputElement>(input_element);
    }

};

}  // namespace detail


namespace policy {
namespace cast {

using DefaultPolicies = policy::DefaultPolicies<1, 1>;

}  // namespace cast
}  // namespace policy


template<
    typename OutputElement,
    typename InputArray>
PartitionedArrayT<InputArray, OutputElement> cast(
    InputArray const& array)
{
    return unary_local_operation(
        policy::cast::DefaultPolicies{},
        array,
        detail::Cast<ElementT<InputArray>, OutputElement>{});
}

}  // namespace lue
