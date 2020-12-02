#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"
#include "lue/framework/algorithm/operator.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"


namespace lue {
namespace detail {

template<
    typename InputElement,
    typename OutputElement_=bool>
class GreaterThanEqualTo
{

public:

    using OutputElement = OutputElement_;

    constexpr OutputElement operator()(
        InputElement const& input_element1,
        InputElement const& input_element2) const noexcept
    {
        return input_element1 >= input_element2;
    }

};

}  // namespace detail


namespace policy {
namespace greater_than_equal_to {

using DefaultPolicies = policy::DefaultPolicies<2, 1>;

}  // namespace greater_than_equal_to
}  // namespace policy


LUE_BINARY_LOCAL_OPERATION_OVERLOADS(greater_than_equal_to, detail::GreaterThanEqualTo)
LUE_BINARY_COMPARISON_OPERATOR(>=, greater_than_equal_to)

}  // namespace lue
