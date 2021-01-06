#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"
#include "lue/framework/algorithm/operator.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"


namespace lue {
namespace detail {

template<
    typename InputElement,
    typename OutputElement_=bool>
class NotEqualTo
{

public:

    using OutputElement = OutputElement_;

    constexpr OutputElement operator()(
        InputElement const& input_element1,
        InputElement const& input_element2) const noexcept
    {
        return input_element1 != input_element2;
    }

};

}  // namespace detail


namespace policy {
namespace not_equal_to {

template<
    typename OutputElement,
    typename InputElement>
using DefaultPolicies = policy::DefaultPolicies<
    OutputElements<OutputElement>,
    InputElements<InputElement, InputElement>>;

}  // namespace not_equal_to
}  // namespace policy


LUE_BINARY_LOCAL_OPERATION_OVERLOADS(not_equal_to, detail::NotEqualTo)
LUE_BINARY_COMPARISON_OPERATOR(!=, not_equal_to)

}  // namespace lue
