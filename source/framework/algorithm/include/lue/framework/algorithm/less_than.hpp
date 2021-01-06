#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"
#include "lue/framework/algorithm/operator.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"


namespace lue {
namespace detail {

template<
    typename InputElement,
    typename OutputElement_=bool>
class LessThan
{

public:

    using OutputElement = OutputElement_;

    constexpr OutputElement operator()(
        InputElement const& input_element1,
        InputElement const& input_element2) const noexcept
    {
        return input_element1 < input_element2;
    }

};

}  // namespace detail


namespace policy {
namespace less_than {

template<
    typename OutputElement,
    typename InputElement>
using DefaultPolicies = policy::DefaultPolicies<
    OutputElements<OutputElement>,
    InputElements<InputElement, InputElement>>;

}  // namespace less_than
}  // namespace policy


LUE_BINARY_LOCAL_OPERATION_OVERLOADS(less_than, detail::LessThan)
LUE_BINARY_COMPARISON_OPERATOR(<, less_than)

}  // namespace lue
